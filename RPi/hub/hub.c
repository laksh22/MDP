#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hub.h"
#include "../tcp/tcp.h"

Queue *b_queue;
Queue *s_queue;

void distribute_command(char *buf, char source) {
  const char s[2] = "!";
  char *point;

  point = strtok(buf, s);

  if (!point) {
    perror(
        "[distribute_command]: Error encountered when splitting received data: ");
  } else {
    while (point != NULL) {
      pthread_mutex_lock(&lock);
      if (source == 't') {
        enqueue(t_queue, point);
      } else if (source == 'b') {
        enqueue(b_queue, point);
      } else if (source == 's') {
        enqueue(s_queue, point);
      }
      pthread_mutex_unlock(&lock);
      point = strtok(NULL, s);
    }
  }
}
void write_hub(char *wpointer, char source) {
  if (wpointer) {
    if (strlen(wpointer) > 0) {
      if (tolower(wpointer[1]) == 't') {

        if (source == 's') {
          *(wpointer + 1) = 's';
        } else if (source == 'b') {
          *(wpointer + 1) = 'b';
        }

        tcp_send(wpointer + 1);
      } else if (tolower(wpointer[1]) == 'b') {
        // TODO: bt_send((void *) wpointer + 2);
      } else if (tolower(wpointer[1]) == 's') {
        // TODO: serial_send((void *) wpointer + 2);
      } else if (tolower(wpointer[1]) == 'r') {

        while (1) {
          // TODO: Implement activate camera
//          if (camera_activate(wpointer + 2)) {
//            tcp_send("rTAKEN");
//            break;
//          }
        }

      } else {
        printf(
            "[write_hub]: Incorrect format provided, message [%s] will be dropped!\n",
            wpointer);
      }
    }
  } else {
    perror("[write_hub]: Error encountered when receiving data to be routed: ");
  }

}
void all_disconnect(int sig) {
  printf(
      "[all_disconnect]: %d signal received, terminating all connection ports!",
      sig);
  tcp_disconnect(tcp_sockfd);
  // TODO: bt_disconnect();
  // TODO: serial_disconnect();
  exit(EXIT_SUCCESS);
}
