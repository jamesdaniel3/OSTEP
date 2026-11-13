#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include "udp.h"

#define BUFFER_SIZE 1024

int main(int argc, char** argv){
    int sd = UDP_Open(10000);
    assert(sd > -1);

    while(1){
        struct sockaddr_in addr;
        char message[BUFFER_SIZE];

        int rc = UDP_Read(sd, &addr, message, BUFFER_SIZE);
        
        printf("Message Received: %s", message);

        if (rc > 0){
            char reply[BUFFER_SIZE];
            sprintf(reply, "Goodbye World!\n");
            rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
        }
    }

    return 0;
}
