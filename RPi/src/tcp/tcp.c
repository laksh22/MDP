#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "tcp.h"
#include "../hub/hub.h"
#include "../settings.h"

struct sockaddr_in servaddr, tcp_client;
socklen_t tcp_opt = sizeof(tcp_client);
int tcp_sockfd, clientconn, isSetOption = 1;

Queue *t_queue;
pthread_mutex_t lock;

int tcp_connect() {
  // Creates an IPv4 two-way endpoint connection for communication
  tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(tcp_sockfd,
             SOL_SOCKET,
             SO_REUSEADDR,
             (char *) &isSetOption,
             sizeof(isSetOption));

  if (tcp_sockfd == -1) {
    perror("[tcp_connect]: Error encountered when creating TCP Socket");
  } else {
    printf("[tcp_connect]: TCP socket has been created successfully!\n");
  }

  // Clears the memory of sockaddr
  bzero(&servaddr, sizeof(servaddr));

  // Assigns the IP and port number to the struct for binding
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  servaddr.sin_port = htons(SERVER_PORT);

  // Binds the newly created socket to the given IP and verifies binding
  if ((bind(tcp_sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
    perror("[tcp_connect]: Error encountered when binding trying to bind");
  } else {
    printf("[tcp_connect]: TCP Socket successfully binded...\n");
  }

  // Configure server to listen for incoming connections
  if (listen(tcp_sockfd, 2) != 0) {
    perror(
            "[tcp_connect]: Error encountered when listening for TCP connections");
    return 0;
  } else {
    printf("[tcp_connect]: TCP Server is now listening...\n");
  }

  // Can shift to become part of the multi-threaded connection
  // Accept the incoming data packet from client and verifies it
  clientconn = accept(tcp_sockfd, (SA *) &tcp_client, &tcp_opt);
  if (clientconn < 0) {
    perror("[tcp_connect]: Error encountered when accepting TCP clients");
    return 0;
  } else {
    printf("[tcp_connect]: TCP Server has successfully accepted the client...\n");
  }
  return 1;
}

void tcp_disconnect(int sock) {
  if (!close(sock)) {
    printf("[tcp_disconnect]: TCP connection id %d closed successfully!\n",
           sock);
  } else {
    perror(
            "[tcp_disconnect]: Error encountered when trying to close TCP connection: ");
  }
}

void tcp_reconnect() {
  int conn = 0;

  while (!conn) {
    printf("[tcp_reconnect]: Attempting to restart tcp connection...\n");
    tcp_disconnect(tcp_sockfd);
    conn = tcp_connect();
    sleep(1);
  }

  printf("[tcp_reconnect]: TCP server connection successfully started!\n");
}

void *tcp_reader_create(void *args) {
  char read_buf[MAX];
  char *rpointer;

  while (1) {
    rpointer = tcp_read();
    if (rpointer) {
      strcpy(read_buf, rpointer);
      memset(rpointer, '\0', MAX);
      distribute_command(read_buf, 't');
    } else {
      perror(
              "[tcp_reader_create]: Error encountered when receiving data from tcp_read");
    }
  }
}

char *tcp_read() {
  int count = 0;
  char tcp_buf[MAX];
  char *p;
  int bytes_read = 0;

  while (1) {
    bytes_read = read(clientconn, &tcp_buf[count], (MAX - count));
    count += bytes_read;
    if (bytes_read > 0) {
      if (strlen(tcp_buf) == 0) {
        continue;
      } else if (tcp_buf[0] == '@') {
        if (tcp_buf[count - 1] != '!') {
          continue;
        } else {
          printf("[tcp_read]: Received [%s] from tcp client connection\n",
                 tcp_buf);
          tcp_buf[count] = '\0';
          p = tcp_buf;
          return p;
        }
      } else {
        printf("[tcp_read]: Invalid string [%s] received, please send a new command\n",
                tcp_buf);
        return NULL;
      }
    } else {
      perror("[tcp_read]: Error encountered when trying to read from TCP");
      tcp_reconnect();
      return NULL;
    }
  }
}

void *tcp_sender_create(void *args) {
  char *q;

  while (1) {
    if (!isEmpty(t_queue)) {
      pthread_mutex_lock(&lock);
      q = dequeue(t_queue);

      write_hub(q, 't');
      pthread_mutex_unlock(&lock);
    }
  }
}

int tcp_send(char *msg) {
  int bytes_write = 0;
  char send[MAX];

  if (strlen(msg) > 0) {
    strcpy(send, msg);
    strcat(send, "\n");
    bytes_write = write(clientconn, send, strlen(send));
    if (bytes_write > 0) {
      printf("[tcp_send]: RPi send message [%s] to PC.\n", send);
      fflush(stdout);
      return 1;
    } else {
      perror("[tcp_send]: Encountered error when RPi tried to send to TCP");
    }
    return 0;
  }
  return 0;
}

