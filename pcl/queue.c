//
// Created by pawci on 04.01.2026.
//

#include "queue.h"

#include <stdlib.h>

// Queue
struct Node {
	void* value;
	struct Node* previous;
	struct Node* next;
};

struct Queue* init() {
	struct Queue *queue = malloc(sizeof(struct Queue));
	if (queue == NULL) {
		return NULL;
	}
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

int isEmpty(struct Queue *queue) {
	if (queue == NULL) {
		return -1;
	}
	if (queue->head == NULL) {
		return TRUE;
	}
	return FALSE;
}

void enqueue(struct Queue *queue, void* value) {
	if (queue == NULL) {
		return;
	}

	struct Node* newNode = malloc(sizeof(struct Node));
	if (newNode == NULL) {
		return;
	}

	newNode->value = value;
	newNode->next = NULL;

	if (queue->head == NULL) {
		newNode->previous = NULL;
		queue->head = newNode;
		queue->tail = newNode;
		return;
	}

	newNode->previous = queue->tail;
	queue->tail->next = newNode;
	queue->tail = newNode;
}

void* dequeue(struct Queue *queue) {
	if (isEmpty(queue)) {
		return 0;
	}
	struct Node* temp = queue->head;
	queue->head = queue->head->next;
	if (queue->head == NULL) {
		queue->tail = NULL;
	}
	void* value = temp->value;
	free(temp);
	return value;
}

void* peek(struct Queue *queue) {
	if (isEmpty(queue)) {
		return 0;
	}
	return queue->head->value;
}