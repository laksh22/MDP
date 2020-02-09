#ifndef RPI_RPI_HUB_HUB_H_
#define RPI_RPI_HUB_HUB_H_

#include <pthread.h>
#include "../queue/queue.h"

extern Queue *b_queue;
extern Queue *s_queue;
extern Queue *t_queue;

extern pthread_mutex_t lock;

// Segment instructions received into individual discrete instructions
void distribute_command(char buf[], char source);

// Route received instructions to required service
void write_hub(char *wpointer, char source);

// Initiate closure of all connection ports/services
void all_disconnect(int sig);

#endif //RPI_RPI_HUB_HUB_H_