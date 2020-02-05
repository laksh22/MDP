#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "queue.h"

struct _Queue {
    int front, rear, size;
    unsigned capacity;
    char **array;
};


Queue *createQueue(unsigned capacity) {
  Queue *queue = (Queue *) malloc(sizeof(Queue));
  queue->capacity = capacity;
  queue->front = queue->size = 0;
  queue->rear = capacity - 1;  // This is important, see the enqueue
  queue->array = (char **) malloc(queue->capacity * sizeof(char *));
  return queue;
}

int isFull(Queue *queue) { return (queue->size == queue->capacity); }

int isEmpty(Queue *queue) { return (queue->size == 0); }

void enqueue(Queue *queue, char *item) {
  if (!isFull(queue)) {
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("Value [%s] enqueued to queue\n", item);
  }
}

char *dequeue(Queue *queue) {
  if (!isEmpty(queue)) {
    char *item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    printf("dequeue: Value [%s] dequeued from queue\n", item);
    return item;
  }

  return 0;
}

char *front(Queue *queue) {
  if (!isEmpty(queue)) {
    return queue->array[queue->front];
  }
  return (char *) INT_MIN;
}

char *rear(Queue *queue) {
  if (!isEmpty(queue)) {
    return queue->array[queue->rear];
  }
  return (char *) INT_MIN;
}
