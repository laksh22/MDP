#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "settings.h"
#include "hub/hub.h"
#include "queue/queue.h"
#include "bluetooth/bluetooth.h"
#include "serial/serial.h"
#include "tcp/tcp.h"

int tcp_status, bt_status, serial_status;
pthread_mutex_t lock;

Queue *b_queue, *s_queue, *t_queue;

int main() {
  // Ctrl+C to terminate the entire program properly
  signal(SIGINT, all_disconnect);
  printf("===== Initializing connections =====\n");

  // Create the respective queues for each devices
  t_queue = createQueue(QSIZE);
  b_queue = createQueue(QSIZE);
  s_queue = createQueue(QSIZE);

  // Default should be 0
  tcp_status = 1;
  bt_status = 1;
  serial_status = 1;

  serial_status = serial_connect();
  bt_status = bt_connect();
  tcp_status = tcp_connect();

  while (!tcp_status || !bt_status || !serial_status) {
    if (!tcp_status) {
      printf("TCP failed to accept client... Retrying in 2s...");
      sleep(2);
      tcp_disconnect(tcp_sockfd);
      tcp_status = tcp_connect();
    } else if (!bt_status) {
      printf("BT connection failed... Retrying in 2s...");
      sleep(2);
      bt_disconnect();
      bt_status = bt_connect();
    } else if (!serial_status) {
      printf("Serial connection failed... Retrying in 2s...");
      sleep(2);
      serial_disconnect();
      serial_status = serial_connect();
    }
  }

  // Create the respective threads to the main program
  pthread_t *thread_group = malloc(sizeof(pthread_t) * NUM_THREADS);
  pthread_create(&thread_group[0], NULL, tcp_reader_create, "\0");
  pthread_create(&thread_group[1], NULL, tcp_sender_create, "\0");
  pthread_create(&thread_group[2], NULL, bt_reader_create, "\0");
  pthread_create(&thread_group[3], NULL, bt_sender_create, "\0");
  pthread_create(&thread_group[4], NULL, serial_reader_create, "\0");
  pthread_create(&thread_group[5], NULL, serial_sender_create, "\0");

  // Join the created threads created previously
  int i;
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(thread_group[i], NULL);
  }

  return 0;
}
