//
// Created by pawci on 04.01.2026.
//

#ifndef PCL_QUEUE_H
#define PCL_QUEUE_H

#include <windows.h>

// Queue
struct Queue {
	struct Node* head;
	struct Node* tail;
};

struct Queue* init();

BOOL isEmpty(struct Queue *queue);

void enqueue(struct Queue *queue, void* value);

void* dequeue(struct Queue *queue);

void* peek(struct Queue *queue);

#endif //PCL_QUEUE_H