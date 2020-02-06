#ifndef RPI_QUEUE_H
#define RPI_QUEUE_H

// Queue Variables
typedef struct _Queue Queue;

// Function to create Queues
Queue *createQueue(unsigned capacity);

// Checks if the queue is full
int isFull(Queue *queue);

// Checks whether the queue is empty
int isEmpty(Queue *queue);

// Adds an item to the end of the queue
void enqueue(Queue *queue, char *item);

// Returns and removes the first item in the queue
char *dequeue(Queue *queue);

// Returns the first item in the queue
char *front(Queue *queue);

// Returns the last item in the queue
char *rear(Queue *queue);

#endif //RPI_QUEUE_H
