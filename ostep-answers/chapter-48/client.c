#include <netinet/in.h>
#include <stdio.h>
#include "udp.h"

#define BUFFER_SIZE 1024

int main(int argc, char** argv){
    int sd = UDP_Open(20000);
    struct sockaddr_in addr_send, addr_recv;
    int rc = UDP_FillSockAddr(&addr_send, "127.0.0.1", 10000);
    char message[BUFFER_SIZE];
    sprintf(message, "Hello world!\n");
    rc = UDP_Write(sd, &addr_send, message, BUFFER_SIZE);
    if (rc > 0){
        rc = UDP_Read(sd, &addr_recv, message, BUFFER_SIZE);
        printf("Message received: %s");
    }

    return 0;
}
