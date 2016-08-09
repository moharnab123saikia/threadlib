#ifndef QUEUE_H
#define QUEUE_H

#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include "mylib.h"

//Structure for queue
struct Node{
	struct _MyThread *curr_thread;
	struct Node *next;
};

typedef struct Queue{
	struct Node *head;
	struct Node *tail;
}Queue;

int is_q_empty(Queue *queue);
void add_to_q(struct _MyThread *thread, Queue *queue);
struct _MyThread* rem_from_q(struct Queue *queue);
int has_thread(struct Queue *queue, struct _MyThread *thread);
struct _MyThread* remove_thread(_MyThread *thread, Queue *queue);
int q_size(Queue *q);

#endif