int my_open(int port);

int my_fill_socket_addr(struct sockaddr_in* addr, char* hostname, int port);

// this was conflicting with socket.h so I just renamed all funcs to have a new convention
int my_send(int sd, struct sockaddr_in* addr, char* bufer, int n);

int my_receive(int sd, struct sockaddr_in* addr, char* buffer, int n);

void await_pending_writes();