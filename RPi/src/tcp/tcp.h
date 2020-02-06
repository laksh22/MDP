#ifndef RPI_RPI_TCP_TCP_H_
#define RPI_RPI_TCP_TCP_H_

#include <sys/socket.h>
#include "../queue/queue.h"

#define SERVER_IP "192.168.17.17"
#define SERVER_PORT 4042

extern struct sockaddr_in servaddr, tcp_client;
extern socklen_t tcp_opt;
extern int tcp_sockfd, clientconn, isSetOption;

// To create and establish TCP connection
int tcp_connect();

// To disconnect a TCP socket
void tcp_disconnect(int sock);

// To reconnect client
void tcp_reconnect();

// To setup thread to read from TCP port
void *tcp_reader_create(void *args);

// To read from the TCP port if data is available
char *tcp_read();

// To setup thread to start writing data received from TCP to devices
void *tcp_sender_create(void *args);

// To send data to device through TCP port
int tcp_send(char *msg);

#endif //RPI_RPI_TCP_TCP_H_
