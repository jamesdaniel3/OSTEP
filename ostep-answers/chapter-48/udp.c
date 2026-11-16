#include <assert.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MIN(X, Y) ((X) < (Y)) ? (X) : (Y)
#define MAX_MESSAGE_LEN 15 // seems like theoretical max len is way longer than anyone supports and most people just set an arbitrary limit
#define STARTING_QUEUE_CAPACITY 10

/*
Unfortunately abandoning ship with this one. The core problem with it is that everything reads from the same socket.
This means that both the receiver which is constantly looping, and the many writes that are all trying to get ACK's,
are all constantly stealing each other's data. I think I put in enough defensive logic for this to prevent things 
from breaking, but the performance is horrid. 

I think to fix it I would really want to start from the top with the async structure in mind and I'm too tired to
do that again. The logic I'm thinking would be this:
- on startup of the server, create a background thread that will loop forever which will be the only consumer of 
    - packets sent to the server
- this thread will read each packet and decide if it's an ack or data
    - if it's an ack, update the relevant write request (haven't really thought through the data structure supporting this)
    - if it's data, call a handle_data function and bascially do what the my_receive function does
- my_receive will now have to block until the most recent message is completed and then return that; bascially splitting out the 
    - wait and return logic from the data update logic 
- the wait function asked for the in the directions would just wait until all pending writes are done 
*/

int my_open(int port){
    int sd;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return -1;
    }

    struct sockaddr_in myaddr;
    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port  = htons(port);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1){
        close(sd);
        return -1;
    }

    return sd;
}

int my_fill_socket_addr(struct sockaddr_in* addr, char* hostname, int port){
    bzero(addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;  // host byte order
    addr->sin_port = htons(port);  // network byte order 
    struct in_addr* in_addr;
    struct hostent* host_entry;

    if ((host_entry = gethostbyname(hostname)) == NULL){  // apparently this function is deprecated 
        return -1;
    }

    in_addr = (struct in_addr*) host_entry->h_addr; // seems like h_addr might not exist idk
    addr->sin_addr = *in_addr;
    return 0;
}

typedef struct MessageChunk MessageChunk;
struct MessageChunk {
    char* text;
    int text_len;
    bool acknowledged;
};

typedef struct Message Message;
struct Message {
    MessageChunk* chunks;
    int total_chunks;
};

typedef struct SenderInfo SenderInfo;
struct SenderInfo {
    pthread_mutex_t lock;
    size_t next_message_id;  // just a counter, mod it to get index
    pthread_t* thread_pool;
};

typedef struct ReceiverQueue ReceiverQueue;
struct ReceiverQueue{
    size_t next_message_to_deliver;
    Message* received_messages;
    size_t capacity;
};

SenderInfo* sender_info = NULL;
ReceiverQueue* receiver_queue = NULL;

void init_sender_info_if_needed(){
    if (sender_info == NULL){
        sender_info = malloc(sizeof(SenderInfo));
        assert(sender_info != NULL);
        pthread_mutex_init(&sender_info->lock, NULL);
        sender_info->next_message_id = 0;
        sender_info->thread_pool = malloc(sizeof(pthread_t) * STARTING_QUEUE_CAPACITY);
    }
}

void init_receiver_queue_if_needed(){
    if (receiver_queue == NULL){
        receiver_queue = malloc(sizeof(ReceiverQueue));
        assert(receiver_queue != NULL);
        receiver_queue->received_messages = calloc(sizeof(Message), STARTING_QUEUE_CAPACITY);
        assert(receiver_queue->received_messages != NULL);
        for(size_t i = 0; i < STARTING_QUEUE_CAPACITY; i++){
            receiver_queue->received_messages[i] = (Message){
                .chunks = NULL,
                .total_chunks = 0
            };
        }
        receiver_queue->next_message_to_deliver = 0;
        receiver_queue->capacity = STARTING_QUEUE_CAPACITY;
    }
}

int my_write(int sd, struct sockaddr_in* addr, char* buffer, int buffer_len, size_t message_id){
    int addr_len = sizeof(struct sockaddr_in);
    int response_addr_len = sizeof(struct sockaddr_in);
    char response_buffer[100]; // holds the ack

    int buffer_index = 0;
    int chunks_created = 0;

    // this upperbound can decrease when we allow the max size of a request to
        // increase
    int chunks_upperbound = (buffer_len / MAX_MESSAGE_LEN) * 5; 
    MessageChunk chunks[chunks_upperbound];

    while(buffer_index < buffer_len){
        // create all of the chunks
        int buffer_chunk_len = MAX_MESSAGE_LEN; // struggling to fix this with dynamix size of metadata
        char* buffer_chunk = calloc(buffer_chunk_len, 1); 
        assert(buffer_chunk != NULL);
        // is_last_chunk:message_id:num_chunks_upperbound:chunk_index:content
        snprintf(
            buffer_chunk, buffer_chunk_len - 1, "0:%zu:%d:%d:", 
            message_id, chunks_upperbound, chunks_created
        ); 
        int prefix_len = strlen(buffer_chunk);
        int bytes_to_read = MIN(buffer_chunk_len - prefix_len, buffer_len - buffer_index);
        memcpy(
            buffer_chunk + prefix_len, 
            buffer + buffer_index, 
            bytes_to_read
        );
        buffer_index += buffer_chunk_len - prefix_len;

        if(buffer_index >= buffer_len){
            buffer_chunk[0] = '1';
        }

        chunks[chunks_created] = (MessageChunk){
            .text = buffer_chunk,
            .text_len = buffer_chunk_len,
            .acknowledged = false
        };
        chunks_created++;
    }

    for(int i = 0; i < chunks_created; i++){
        // send all of the chunks 
        MessageChunk current_chunk = chunks[i];
        sendto(
            sd, 
            current_chunk.text, 
            current_chunk.text_len, 
            0, 
            (struct sockaddr*)addr, 
            addr_len
        );
    }

    int chunks_acked = 0;
    while (chunks_acked < chunks_created){
        // confirm and resend chunks 
        sleep(1);   // 1 second is too long to sleep for but for now it's fine   
        
        int rc = 0;
        while (rc != -1){
            rc = recvfrom(
                sd, response_buffer, sizeof(response_buffer), MSG_DONTWAIT | MSG_PEEK, 
                (struct sockaddr*)addr, (socklen_t*)&response_addr_len
            );

            if (rc == -1 || strncmp(response_buffer, "ack", 3) != 0){
                continue;
            }

            char* next = NULL;
            // assuming ack:id:index format
            size_t found_message_id = strtol(response_buffer + 4, &next, 10);
            assert(found_message_id >= 0);

            if (found_message_id != message_id){
                continue;
            }

            // clear the message because we know it was meant to be read
            rc = recvfrom(
                sd, response_buffer, sizeof(response_buffer), MSG_DONTWAIT, 
                (struct sockaddr*)addr, (socklen_t*)&response_addr_len
            );

            int chunk_index = strtol(next + 1, &next, 10);
            MessageChunk* current_chunk = chunks + chunk_index;

            if(current_chunk->acknowledged){
                continue;
            }

            current_chunk->acknowledged = true;
            free(current_chunk->text);
            chunks_acked++;
        }

        for(int i = 0; i < chunks_created; i++){
            MessageChunk current_chunk = chunks[i];
            if (current_chunk.acknowledged == false){
                sendto(
                    sd, 
                    current_chunk.text, 
                    current_chunk.text_len, 
                    0, 
                    (struct sockaddr*)addr, 
                    addr_len
                );
            }
        }
    }

    free(buffer); // this is a copy we made

    // sendto returns the number of bytes sent. Unsure if this functino should send
    // the actual number of bytes sent (include metadata) or the number of bytes the
    // user asked to send 
    return buffer_len; 
}

typedef struct MyWriteArgs MyWriteArgs;
struct MyWriteArgs {
    int sd;
    struct sockaddr_in* addr;
    char* buffer;
    int buffer_len;
    size_t message_id;
};

void* my_write_wrapper(void* args){
    MyWriteArgs args_unpacked = *(MyWriteArgs*)args;
    free(args);
    my_write(
        args_unpacked.sd, 
        args_unpacked.addr, 
        args_unpacked.buffer, 
        args_unpacked.buffer_len, 
        args_unpacked.message_id
    );

    return NULL;
}

void my_send(int sd, struct sockaddr_in* addr, char* buffer, int buffer_len){
    init_sender_info_if_needed();

    pthread_mutex_lock(&sender_info->lock);
    size_t message_id_to_send = sender_info->next_message_id;
    sender_info->next_message_id++;
    pthread_mutex_unlock(&sender_info->lock);

    // pending message capacity will need to be accessed through a lock once it can actually be changed
    size_t pending_message_index = message_id_to_send % STARTING_QUEUE_CAPACITY;
    MyWriteArgs* thread_args = malloc(sizeof(MyWriteArgs));
    assert(thread_args != NULL);

    char* buffer_copy = malloc(buffer_len);
    assert(buffer_copy != NULL);
    memcpy(buffer_copy, buffer, buffer_len);

    thread_args->sd = sd;
    thread_args->addr = addr;
    thread_args->buffer = buffer_copy;
    thread_args->buffer_len = buffer_len;
    thread_args->message_id = message_id_to_send;

    int rc = pthread_create(
        &sender_info->thread_pool[pending_message_index],
        NULL,
        my_write_wrapper,
        thread_args
    );
    assert(rc == 0);
}

int my_receive(int sd, struct sockaddr_in* addr, char* result_buffer, int result_buffer_len){
    int rc = 0;
    size_t message_id = 0;
    int is_last_chunk, current_chunk_index, num_chunks_upperbound = 0;
    Message* current_message = NULL;

    init_receiver_queue_if_needed();

    while (true){
        char result_chunk[MAX_MESSAGE_LEN];
        int len = sizeof(struct sockaddr_in);
        rc = recvfrom(
            sd, 
            result_chunk, 
            MAX_MESSAGE_LEN, 
            MSG_PEEK, 
            (struct sockaddr*)addr, 
            (socklen_t*)&len
        );

        if (strncmp(result_chunk, "ack", 3) == 0){
            sleep(1);
            continue;
        }

        // consume the message
        rc = recvfrom(
            sd, 
            result_chunk, 
            MAX_MESSAGE_LEN, 
            0, 
            (struct sockaddr*)addr, 
            (socklen_t*)&len
        );

        char* next = result_chunk;
        is_last_chunk = strtol(result_chunk, &next, 10);
        message_id = strtol(next + 1, &next, 10);
        num_chunks_upperbound = strtol(next + 1, &next, 10);
        current_chunk_index = strtol(next + 1, &next, 10);
        result_chunk[MAX_MESSAGE_LEN - 1] = '\0';

        char ack_buffer[20]; // techincally could hit an upperbound on pages but would expect other problems first
        snprintf(ack_buffer, 19, "ack:%zu:%d", message_id, current_chunk_index); 
        sendto(
            sd, 
            ack_buffer, 
            strlen(ack_buffer), 
            0, 
            (struct sockaddr*)addr, 
            sizeof(struct sockaddr_in)
        );

        // no lock needed because the locking behavior in send()
        // ensures that this spot in the message queue is available
        size_t receiever_queue_index = message_id % receiver_queue->capacity;
        current_message = &receiver_queue->received_messages[receiever_queue_index];

        if (current_message->chunks == NULL){
            current_message->chunks = calloc(num_chunks_upperbound, sizeof(MessageChunk));
            assert(current_message->chunks != NULL);
        }

        if (is_last_chunk){
            current_message->total_chunks = current_chunk_index + 1;
        }

        MessageChunk* current_chunk = current_message->chunks + current_chunk_index;

        // this is uninitialized 
        if (current_chunk->acknowledged == true){
            continue;
        }

        current_chunk->text = calloc(MAX_MESSAGE_LEN, 1); 
        assert(current_chunk->text != NULL);
        int info_prefix_len = next + 1 - result_chunk;
        memcpy(current_chunk->text, next + 1, MAX_MESSAGE_LEN - info_prefix_len); 
        current_chunk->text_len = strlen(current_chunk->text); 
        current_chunk->acknowledged = true;

        bool is_first_message_in_queue = receiver_queue->next_message_to_deliver == message_id;

        if (
            // can't stop reading until the most recent message is completely read
            is_first_message_in_queue &&
            current_message != NULL && 
            current_message->total_chunks != 0
        ){
            bool awaiting_packets = false;
            for(int i = 0; i < current_message->total_chunks; i++){
                MessageChunk current_chunk = current_message->chunks[i];
                if (current_chunk.acknowledged == false){
                    awaiting_packets = true;
                    break;
                }
            }

            if (!awaiting_packets){
                break;
            }
        }
    }

    int current_result_index = 0;
    int chunk_copying_index = 0;
    while (
        current_result_index < result_buffer_len &&
        chunk_copying_index < current_message->total_chunks
    ){
        MessageChunk current_chunk = current_message->chunks[chunk_copying_index];
        size_t num_chars_to_copy = MIN(
            result_buffer_len - current_result_index + 1, 
            current_chunk.text_len
        );

        // only copy what can fit in the buffer; save space for the null terminator
        memcpy(
            result_buffer + current_result_index, 
            current_chunk.text, 
            num_chars_to_copy
        );

        free(current_chunk.text);
        current_result_index += num_chars_to_copy;
        chunk_copying_index++;
    } 

    free(current_message->chunks);
    current_message->chunks = NULL;
    receiver_queue->next_message_to_deliver++;

    return rc; // this is the wrong value
}
