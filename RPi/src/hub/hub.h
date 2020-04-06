#ifndef RPI_RPI_HUB_HUB_H_
#define RPI_RPI_HUB_HUB_H_

#include <pthread.h>
#include "../rpa_queue/rpa_queue.h"

extern rpa_queue_t *b_queue;
extern rpa_queue_t *s_queue;
extern rpa_queue_t *t_queue;
extern rpa_queue_t *r_queue;

//extern pthread_mutex_t lock;

// Segment instructions received into individual discrete instructions
void distribute_command(char buf[], char source);

// Route received instructions to required service
void write_hub(char *wpointer, char source);

// Initiate closure of all connection ports/services
void all_disconnect(int sig);

#endif //RPI_RPI_HUB_HUB_H_
