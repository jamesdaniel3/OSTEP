int UDP_Open(int port);

int UDP_FillSockAddr(struct sockaddr_in* addr, char* hostname, int port);

int UDP_Write(int sd, struct sockaddr_in* addr, char* bufer, int n);

int UDP_Read(int sd, struct sockaddr_in* addr, char* buffer, int n);