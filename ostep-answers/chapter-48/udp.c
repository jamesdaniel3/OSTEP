#include <netdb.h>
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MIN(X, Y) ((X) < (Y)) ? (X) : (Y)
#define MAX_MESSAGE_LEN 10 // need to lookup how big this actually should be 

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

int UDP_Write(int sd, struct sockaddr_in* addr, char* buffer, int buffer_len){
    int addr_len = sizeof(struct sockaddr_in);
    int response_addr_len = sizeof(struct sockaddr_in);
    char response_buffer[1024]; // holds the ack

    int ret;
    int buffer_index = 0;
    int buffer_chunk_index = 0;
    while(buffer_index < buffer_len){
        size_t buffer_chunk_len = MIN(MAX_MESSAGE_LEN, buffer_len - buffer_index + 4);
        char* buffer_chunk = calloc(buffer_chunk_len, 1);   // can probs not use calloc here
        sprintf(buffer_chunk, "0:%d:", buffer_chunk_index); // is_last_piece:piece_index:content
        size_t prefix_len = strlen(buffer_chunk);
        memcpy(
            buffer_chunk + prefix_len, 
            buffer + buffer_index, 
            buffer_chunk_len - prefix_len
        );
        buffer_index += buffer_chunk_len - prefix_len;
        if (buffer_index == buffer_len){
            buffer_chunk[0] = '1';
        }

        while (1){
            ret = sendto(sd, buffer_chunk, buffer_chunk_len, 0, (struct sockaddr*)addr, addr_len);
            sleep(1);   // 1 second is too long to sleep for but for now it's fine        
            int rc = recvfrom(
                sd, response_buffer, 1024, MSG_DONTWAIT, 
                (struct sockaddr*)addr, (socklen_t*)&response_addr_len
            );

            if (rc >= 0){
                break;
            }
        }

        free(buffer_chunk);
        buffer_chunk_index++;
    }

    return ret;
}

int UDP_Read(int sd, struct sockaddr_in* addr, char* result_buffer, int result_buffer_len){
    int last_piece_seen = -1; // this will have to change when we stop sending packets in order
    int rc, is_last_piece, current_piece, current_result_index = 0;
    while (1){
        char result_chunk[MAX_MESSAGE_LEN];
        int len = sizeof(struct sockaddr_in);
        rc = recvfrom(sd, result_chunk, MAX_MESSAGE_LEN, 0, (struct sockaddr*)addr, (socklen_t*)&len);

        char ack_buffer[4] = "ack";
        sendto(sd, ack_buffer, 4, 0, (struct sockaddr*)addr, sizeof(struct sockaddr_in));

        char* next = result_chunk;
        is_last_piece = strtol(result_chunk, &next, 10);
        current_piece = strtol(next + 1, &next, 10);

        if (current_piece == last_piece_seen){
            continue;
        }
        last_piece_seen = current_piece;

        int info_prefix_len = next + 1 - result_chunk;
        size_t num_chars_to_copy = MIN(result_buffer_len - current_result_index + 1, MAX_MESSAGE_LEN - info_prefix_len);

        // only copy what can fit in the buffer; save space for the null terminator
        memcpy(result_buffer + current_result_index, next + 1, num_chars_to_copy);
        current_result_index += num_chars_to_copy;

        if (is_last_piece){
            break;
        }
    }

    return rc;
}
