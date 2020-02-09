#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringSerial.h>
#include "../settings.h"
#include "../hub/hub.h"

int fd_serial;

int serial_connect() {
  fd_serial = serialOpen(SERIAL_PORT, BAUD);
  if (fd_serial == -1) {
    perror("[serial_connect]: Error encounted when establishing serial port connection");
    return 0;
  } else {
    printf("[serial_connect]: Serial port connection %s with %d established successfully.\n",
           SERIAL_PORT, BAUD);
    return 1;
  }
}

void serial_disconnect() {
  serialClose(fd_serial);
  printf("[serial_disconnect]: Serial port connection closed successfully!\n");
}

void serial_reconnect() {
  int conn = 0;

  while (!conn) {
    printf("[serial_reconnect]: Attempting to re-establish connection to serial port...\n");
    serial_disconnect();
    conn = serial_connect();
    sleep(1);
  }

  printf("[serial_reconnect]: Serial connection successfully reconnected!\n");
}

char *serial_read() {
  int bytes_read;
  int count = 0;
  char serial_buf[MAX];
  char new_char;
  char *p;

  while (1) {
    bytes_read = serialDataAvail(fd_serial);
    if (bytes_read > 0) {
      new_char = serialGetchar(fd_serial);
      if (new_char == '\n') {
        if (serial_buf[0] == '@') {
          if (serial_buf[(count - 2)] == '!') {
            serial_buf[(count - 1)] == '\0';
            printf("[serial_read]: Received [%s] from Serial client connection\n",
                   serial_buf);
            p = serial_buf;
            return p;
          } else {
            continue;
          }
        } else {
          printf("[serial_read]: Invalid string [%s] received, please send a new command\n",
                 serial_buf);
          return 0;
        }
      } else {
        serial_buf[count] = new_char;
        count++;
      }
    } else if (bytes_read < 0) {
      perror("[serial_read]: Error encountered when trying to read from serial");
      serial_reconnect();
      return 0;
    } else {
      // bytes_read == 0
      continue;
    }
  }
}

void *serial_reader_create(void *args) {
  char read_buf[MAX];
  char *rpointer;

  while (1) {
    rpointer = serial_read();
    if (rpointer) {
      strcpy(read_buf, rpointer);
      memset(rpointer, '\0', MAX);
      distribute_command(read_buf, 's');
    } else {
      perror("[serial_reader_create]: Error encountered when receiving data from serial_read");
    }
  }
}

int serial_send(char *msg) {
  char send[MAX];
  if (strlen(msg) > 0) {
    strcpy(send, msg);
    strcat(send, "\n");
    serialPuts(fd_serial, send);
    printf("[serial_send]: RPi send message [%s] to serial.\n", msg);
    fflush(stdout);
    return 1;
  }
  return 0;
}

void *serial_sender_create(void *args) {
  char *q;

  // Endless loop
  while (1) {
    if (!isEmpty(s_queue)) {
      pthread_mutex_lock(&lock);
      q = dequeue(s_queue);
      printf("[serial_send_create]: [%s] dequeued from serial queue\n", q);
      write_hub(q, 's');
      pthread_mutex_unlock(&lock);
    }
  }
}