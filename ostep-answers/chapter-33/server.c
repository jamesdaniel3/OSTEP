#include <stdio.h>
#include <unistd.h> // read and write
#include <netinet/in.h> // sockaddr_in and related structs
#include <sys/socket.h> // socket functions
#include <assert.h> 
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 3000
#define MAX_BACKLOG 8
#define MAX_REQUEST_LEN 256
#define MAX_RESPONSE_LEN 4000

int get_listener_socket(){
    struct sockaddr_in addr;

    // create a socket
    // should this be AF_INET?
    // https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
    int descriptor = socket(PF_INET, SOCK_STREAM, 0); 
    assert(descriptor > 0);

    // htons and htonl are used because the internet expects big endian storage 
    // and the host that is running this program may be little endian
    // s vs l means __________________
    addr.sin_family = AF_INET; // use IPv4 addr I think ???
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // do not bind to a specific IP
    addr.sin_port = htons(SERVER_PORT); // only accept requests targeting SERVER_PORT

    if(bind(descriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        close(descriptor);
        return -1;
    }

    if(listen(descriptor, MAX_BACKLOG) < 0){
        return -1;
    }

    return descriptor;
}

void handle_request(
    size_t* num_descriptors, struct pollfd* descriptors, 
    size_t* fd_index
){
    char request[MAX_REQUEST_LEN];
    char response[MAX_RESPONSE_LEN + 1];
    int client_descriptor = descriptors[*fd_index].fd;
    int bytes_read = read(client_descriptor, request, MAX_REQUEST_LEN);
   
    if (bytes_read <= 0){
        if (bytes_read == 0){
            printf("Socket hung up\n");
        }

        close(descriptors[*fd_index].fd);

        descriptors[*fd_index] = descriptors[*num_descriptors - 1];
        (*num_descriptors)--;
        (*fd_index)--;
        return;
    }

    if (request[bytes_read - 1] == '\n'){
        request[bytes_read - 1] = '\0';
    } 
    else {
        request[bytes_read] = '\0';
    }

    // should probs use dynamic buffer so we can read any size message and write any size response
    printf("Message received: %s\n", request);

    int requested_file_fd = open(request, O_RDONLY);
    int file_bytes_read;
    if (requested_file_fd < 0){
        if (errno == ENOENT){
            snprintf(response, MAX_RESPONSE_LEN, "File not found");    
        }
        else {
            exit(1);
        }
    }
    else{
        // should error check
        file_bytes_read = read(requested_file_fd, response, MAX_RESPONSE_LEN);
    }

    close(requested_file_fd);
    write(client_descriptor, response, file_bytes_read); // should check if this fails
}

void create_new_connection(
    int listener, size_t* num_descriptors, 
    size_t* descriptors_cap, struct pollfd** descriptors
){
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    client_addr_len = sizeof(client_addr); // why do I need this??
    int client_descriptor = accept(
        listener, 
        (struct sockaddr *)&client_addr,
        &client_addr_len
    );
    assert(client_descriptor > 0);

    if (
        *num_descriptors >= *descriptors_cap * .8 ||
        *num_descriptors - 2 >= *descriptors_cap
    ){
        *descriptors = realloc(*descriptors, sizeof(**descriptors) * (*descriptors_cap * 2));
        assert(descriptors != NULL);
        *descriptors_cap *= 2;
    }

    (*descriptors)[*num_descriptors].fd = client_descriptor;
    (*descriptors)[*num_descriptors].events = POLLIN;
    (*descriptors)[*num_descriptors].revents = 0; // what does this do


    (*num_descriptors)++;
}

void handle_connections(
    int listener, size_t* num_descriptors, 
    size_t* descriptors_cap, struct pollfd** descriptors
){
    printf("Function called %zu\n", *num_descriptors);
    for (size_t i = 0; i < *num_descriptors; i++){
        if ((*descriptors)[i].revents & (POLLIN | POLLHUP)) {
            if((*descriptors)[i].fd == listener){
               create_new_connection(listener, num_descriptors, descriptors_cap, descriptors);
            }
            else {
                handle_request(num_descriptors, *descriptors, &i);
            }
        }
    }
}

int main(){
    size_t descriptors_capacity = 5;
    size_t num_descriptors= 0;
    struct pollfd *descriptors = calloc(5, sizeof(struct pollfd));
    assert(descriptors != NULL);
    num_descriptors++;

    int listener = get_listener_socket();
    assert(listener != -1);

    // add listener to set 
    descriptors[0].fd = listener;
    descriptors[0].events = POLLIN; // surprised we don't also need POLLOUT

    printf("Server started! Wating for connections on %d...\n", SERVER_PORT);

    while(true){
        int poll_result = poll(descriptors, num_descriptors, -1); // block forever
        assert(poll_result != -1);

        handle_connections(listener, &num_descriptors, &descriptors_capacity, &descriptors);
    }
    
    free(descriptors);
    return 0;
}
