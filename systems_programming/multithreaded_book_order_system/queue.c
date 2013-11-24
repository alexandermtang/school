#include <stdlib.h>

#include "queue.h"

Queue *Q_create_queue() {
	Queue *queue = malloc(sizeof(struct Queue));
	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	return queue;
}

void Q_enqueue(Queue *queue, void *data) {
	QueueNode *node = malloc(sizeof(struct QueueNode));
	node->data = data;
	node->next = NULL;

	if (queue->length == 0) {
		queue->head = node;
		queue->tail = node;
		queue->length++;
		return;
	}

	queue->tail->next = node;
	queue->tail = node;
	queue->length++;
}

void *Q_dequeue(Queue *queue) {
	if (queue->length == 1) {
		QueueNode *head = queue->head;
		void *data = head->data;
		free(head);
		queue->length = 0;
		return data;
	}

	QueueNode *head = queue->head;
	void *data = head->data;
	queue->head = queue->head->next;
	free(head);				// may need to do free(head->next) before this
	queue->length--;

	return data;
}

int Q_length(Queue *queue) {
	return queue->length;
}

void Q_destroy(Queue *queue) {
	while (queue->length > 0) {
		Q_dequeue(queue);
	}
}
