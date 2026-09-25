#include <stdio.h>
#include <unistd.h> // read and write
#include <netinet/in.h> // sockaddr_in and related structs
#include <sys/socket.h> // socket functions
#include <assert.h> 

#define SERVER_PORT 3000
#define MAX_BACKLOG 8
#define MAX_MESSAGE_LEN 4000

int main(){
    char buffer[MAX_MESSAGE_LEN];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    // create a socket
    // should this be AF_INET?
    // https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
    int server_descriptor = socket(PF_INET, SOCK_STREAM, 0); 
    assert(server_descriptor > 0);

    // htons and htonl are used because the internet expects big endian storage 
    // and the host that is running this program may be little endian
    // s vs l means __________________

    server_addr.sin_family = AF_INET; // use IPv4 addr I think ???
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // do not bind to a specific IP
    server_addr.sin_port = htons(SERVER_PORT); // only accept requests targeting SERVER_PORT

    bind(server_descriptor, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_descriptor, MAX_BACKLOG);

    printf("Server started! Wating for connections on %d...\n", SERVER_PORT);

    int bytes_read;
    while(true){
        client_addr_len = sizeof(client_addr); // why do I need this??
        int client_descriptor = accept(
            server_descriptor, 
            (struct sockaddr *)&client_addr,
            &client_addr_len
        );

        if(client_descriptor < 0){
            printf("Client accept attempted but failed.\n");
            continue;
        }

        // should probs create a dynamic buffer so we can read any size message 
        while((bytes_read = read(client_descriptor, buffer, MAX_MESSAGE_LEN)) > 0){
            printf("Message received: %s\n", buffer);
            write(client_descriptor, buffer, MAX_MESSAGE_LEN); // should check if this fails
        }

        if (bytes_read < 0){
            printf("Read error\n");
            continue;
        }
        close(client_descriptor);
    }

    close(server_descriptor);
    return 0;
}
