#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include "queue.h"

//queue functions

int is_q_empty(Queue *queue){

	if(queue->head == NULL && queue->tail == NULL){
		return 1;
	}
	else
		return 0;
}

void add_to_q(_MyThread *thread, Queue *queue){

	struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));

	new_node->curr_thread = thread;
	new_node->next = NULL;

	if(is_q_empty(queue)){
		queue->head=queue->tail=new_node;
		return;
	}
	queue->tail->next = new_node;
	queue->tail = new_node;

}

_MyThread* rem_from_q(Queue *queue){

	_MyThread *old_thread;

	if(is_q_empty(queue)){
		return;
	}

	old_thread = queue->head->curr_thread;

	if(queue->head==queue->tail){

		queue->head = queue->tail = NULL;
	}
	else
		queue->head = queue->head->next;

	return old_thread;

}

int has_thread(Queue *queue, _MyThread *thread){

	struct Node *temp;
	temp = queue->head;

	if(temp == NULL)
		return 0;
	while(temp->curr_thread != thread){
		temp = temp->next;
		if(temp == NULL)
			return 0;
	}

	return 1;
}

struct _MyThread* remove_thread(_MyThread *thread, Queue *queue){
	struct Node *temp = queue->head;
	if(temp !=NULL){
		if(temp->curr_thread == thread){
			if(queue->head == queue->tail){
				queue->head = NULL;
				queue->tail = NULL;
			}
			else
			{
				queue->head = queue->head->next;
			}
			return temp->curr_thread;
		}
		else{
			struct Node *start = queue->head;
			temp = temp->next;
			while(temp!= NULL){
				if(temp->curr_thread == thread){
					start->next = temp->next;
					return temp->curr_thread;
				}
				temp = temp->next;
				start = start->next;
			}
		}
	}
}

int q_size(Queue *q) {
	int size=0;
	struct Node *p= q->head;
	while(p != NULL) {
		size++;
		p = p->next;
	}
	return size;	
}

void print_q(Queue *q) {
	struct Node *p= q->head;
	printf("\n_________________________");
	while(p != NULL) {
		printf("\nThread : %u", p->curr_thread);
		p = p->next;
		
	}
	printf("\n_________________________");
	return;	
}

