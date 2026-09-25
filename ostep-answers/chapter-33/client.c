#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // needed for inet_addr function
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SERVER_PORT 3000
#define MAX_BUFFER_LEN 4000

int main(int argc, char** argv){
    struct sockaddr_in server_addr;
    char request[MAX_BUFFER_LEN];
    char response[MAX_BUFFER_LEN];

    if (argc != 2){
        printf("Include the IP address of the server in args and only that!\n");
        exit(1);
    }

    int client_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (client_descriptor < 0) {
        printf("Failed to create client socket!\n");
        exit(1);
    }

    // why didn't we init this on the server?
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // need to look up this function
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_descriptor, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Failed to connect to server!\n");
        exit(1);
    }

    while (fgets(request, MAX_BUFFER_LEN, stdin) != NULL){
        write(client_descriptor, request, MAX_BUFFER_LEN);

        read(client_descriptor, response, MAX_BUFFER_LEN);

        printf("Response received: %s\n", response);
    }

    return 0;
}
