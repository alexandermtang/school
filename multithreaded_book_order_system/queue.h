#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct QueueNode* QueueNodePtr;

typedef struct QueueNode {
	void *data;
	QueueNodePtr next;
} QueueNode;

typedef struct Queue {
	int isopen; // Used to signal when producer is done producing
	QueueNode *head;
	QueueNode *tail;
	int length;
	pthread_mutex_t mutex;
	pthread_cond_t dataAvailable;
	pthread_cond_t spaceAvailable;
} Queue;

Queue *Q_create_queue();
void Q_enqueue(Queue *queue, void *data);
void *Q_dequeue(Queue *queue);
int Q_length(Queue *queue);
void Q_destroy(Queue *queue);

#endif
