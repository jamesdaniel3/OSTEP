#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "udp.h"

#define BUFFER_SIZE 1024

int main(){
    int sd = UDP_Open(20000);
    struct sockaddr_in addr_send, addr_recv;
    int rc = UDP_FillSockAddr(&addr_send, "127.0.0.1", 10000);

    char message[28];
    snprintf(message, 21, "0123456789ABCDEFGHIJ");
    rc = UDP_Write(sd, &addr_send, message, 21);

    if (rc > 0){
        bzero(message, 28);
        rc = UDP_Read(sd, &addr_recv, message, BUFFER_SIZE);
        printf("Client message received: %s\n", message);
    }

    return 0;
}
