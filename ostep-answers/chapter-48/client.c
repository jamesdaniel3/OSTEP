#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "udp.h"

#define BUFFER_SIZE 1024

int main(){
    int sd = my_open(20000);
    struct sockaddr_in addr_send, addr_recv;
    my_fill_socket_addr(&addr_send, "127.0.0.1", 10000);

    char message[28];
    snprintf(message, 15, "Hello World!");
    my_send(sd, &addr_send, message, 15);

    // sleep(10);
    bzero(message, 28);
    my_receive(sd, &addr_recv, message, 28);
    printf("Client message received: %s\n", message);

    return 0;
}
