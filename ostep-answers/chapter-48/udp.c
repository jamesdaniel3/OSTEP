#include <assert.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MIN(X, Y) ((X) < (Y)) ? (X) : (Y)
#define MAX_MESSAGE_LEN 10 // seems like theoretical max len is way longer than anyone supports and most people just set an arbitrary limit

int UDP_Open(int port){
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

int UDP_FillSockAddr(struct sockaddr_in* addr, char* hostname, int port){
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
struct MessageChunk{
    char* text;
    int text_len;
    bool acknowledged;
};

int UDP_Write(int sd, struct sockaddr_in* addr, char* buffer, int buffer_len){
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
        size_t buffer_chunk_len = MAX_MESSAGE_LEN; // struggling to fix this with dynamix size of metatdata
        char* buffer_chunk = calloc(buffer_chunk_len, 1);   // probs don't have to use calloc here
        // is_last_chunk:num_chunks_upperbound:chunk_index:content
        snprintf(buffer_chunk, buffer_chunk_len - 1, "0:%d:%d:", chunks_upperbound, chunks_created); 
        size_t prefix_len = strlen(buffer_chunk);
        memcpy(
            buffer_chunk + prefix_len, 
            buffer + buffer_index, 
            buffer_chunk_len - prefix_len
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
                sd, response_buffer, 1024, MSG_DONTWAIT, 
                (struct sockaddr*)addr, (socklen_t*)&response_addr_len
            );

            if (rc == -1){
                continue;
            }

            int chunk_index = strtol(response_buffer + 4, NULL, 10); // assuming ack:index format
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

    // sendto returns the number of bytes sent. Unsure if this functino should send
    // the actual number of bytes sent (include metadata) or the number of bytes the
    // user asked to send 
    return buffer_len; 
}

int UDP_Read(int sd, struct sockaddr_in* addr, char* result_buffer, int result_buffer_len){
    int rc, is_last_chunk, current_chunk_index, num_chunks_upperbound, total_chunks = 0;
    MessageChunk* chunks = NULL;
    while (true){
        char result_chunk[MAX_MESSAGE_LEN];
        int len = sizeof(struct sockaddr_in);
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
        num_chunks_upperbound = strtol(next + 1, &next, 10);
        current_chunk_index = strtol(next + 1, &next, 10);

        char ack_buffer[10]; // techincally could hit an upperbound on pages but would expect other problems first
        snprintf(ack_buffer, 9, "ack:%d", current_chunk_index); 
        sendto(
            sd, 
            ack_buffer, 
            strlen(ack_buffer), 
            0, 
            (struct sockaddr*)addr, 
            sizeof(struct sockaddr_in)
        );

        if (chunks == NULL){
            chunks = malloc(num_chunks_upperbound * sizeof(MessageChunk));
            assert(chunks != NULL);
        }

        if (is_last_chunk){
            total_chunks = current_chunk_index + 1;
        }

        MessageChunk* current_chunk = chunks + current_chunk_index;

        if (current_chunk->acknowledged == true){
            continue;
        }

        current_chunk->text = calloc(1, MAX_MESSAGE_LEN);  // this is too big
        assert(current_chunk->text != NULL);
        int info_prefix_len = next + 1 - result_chunk;
        memcpy(current_chunk->text, next + 1, MAX_MESSAGE_LEN - info_prefix_len);  // also too big when buffer not full
        // this is okay maybe, but if we want to do this we 
        // should probs make sure the sender sends every message at MAX_MESSAGE_LEN
        current_chunk->text_len = strlen(current_chunk->text); 
        current_chunk->acknowledged = true;

        if (total_chunks != 0){
            bool awaiting_packets = false;
            for(int i = 0; i < total_chunks; i++){
                MessageChunk current_chunk = chunks[i];
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
        chunk_copying_index < total_chunks
    ){
        MessageChunk current_chunk = chunks[chunk_copying_index];
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

    return rc; // this is the wrong value
}
