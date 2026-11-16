#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include "udp.h"

#define BUFFER_SIZE 1024

int main(){
    int sd = my_open(10000);
    assert(sd > -1);

    while(1){
        struct sockaddr_in addr;
        char message[BUFFER_SIZE];

        int rc = my_receive(sd, &addr, message, BUFFER_SIZE);
        
        printf("Server message received: %s\n", message);

        if (rc > 0){
            char reply[BUFFER_SIZE];
            snprintf(reply, 15, "Goodbye World!");
            rc = my_send(sd, &addr, reply, 15);
        }
    }
}
