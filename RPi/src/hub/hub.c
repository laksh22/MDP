#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hub.h"
#include "../bluetooth/bluetooth.h"
#include "../serial/serial.h"
#include "../tcp/tcp.h"
#include "../camera/camera.h"

rpa_queue_t *b_queue, *s_queue, *t_queue, *r_queue;

// Blocking rpa_queue implementation
void distribute_command(char *buf, char source) {
  const char s[2] = "!";
  char *point;

  point = strtok(buf, s);

  if (!point) {
    perror(
            "[distribute_command]: Error encountered when splitting received data");
    fflush(stdout);
  } else {
    while (point != NULL) {
      if (source == 't') {
        rpa_queue_push(t_queue, point);
      } else if (source == 'b') {
        rpa_queue_push(b_queue, point);
      } else if (source == 's') {
        rpa_queue_push(s_queue, point);
      }
      point = strtok(NULL, s);
    }
  }
}

void write_hub(char *wpointer, char source) {
  if (wpointer) {
    if (strlen(wpointer) > 0) {
      if (tolower(wpointer[1]) == 't') {

        // Uncomment if TCP needs to know the source
//        if (source == 's') {
//          *(wpointer + 1) = 's';
//        } else if (source == 'b') {
//          *(wpointer + 1) = 'b';
//        }
        tcp_send(wpointer + 2);

      } else if (tolower(wpointer[1]) == 'b') {

        bt_send((void *) wpointer + 2);

      } else if (tolower(wpointer[1]) == 's') {
        if (source == 't') {
          *(wpointer + 1) = 't';
          serial_send((void *) wpointer + 1);
        } else if (source == 'b') {
          *(wpointer + 1) = 'b';
          serial_send((void *) wpointer + 1);
        } else if (source == 'p') {
          // Send keep alive message
          *(wpointer + 1) = 'p';
          serial_send((void *) wpointer + 1);
        } else {
          printf("[write_hub] Unknown source received: [%s]\n", source);
        }

      } else if (tolower(wpointer[1]) == 'r') {

        rpa_queue_push(r_queue, wpointer + 2);

      } else {
        printf("[write_hub]: Incorrect recipient!\n");
        fflush(stdout);
      }
    }
  } else {
    perror("[write_hub]: Error encountered when receiving data to be routed");
    fflush(stdout);
  }

}

void all_disconnect(int sig) {
  printf("[all_disconnect]: %d signal received, terminating all connection ports!\n",
         sig);
  fflush(stdout);
  tcp_disconnect(tcp_sockfd);
  bt_disconnect();
  serial_disconnect();
  exit(EXIT_SUCCESS);
}
