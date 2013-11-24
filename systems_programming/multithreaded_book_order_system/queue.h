#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode* QueueNodePtr;

typedef struct QueueNode {
	void *data;
	QueueNodePtr next;
} QueueNode;

typedef struct Queue {
	QueueNode *head;
	QueueNode *tail;
	int length;
} Queue;

Queue *Q_create_queue();
void Q_enqueue(Queue *queue, void *data);
void *Q_dequeue(Queue *queue);
int Q_length(Queue *queue);
void Q_destroy(Queue *queue);

#endif
