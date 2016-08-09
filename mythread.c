#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include "mythread.h"
#include "queue.h"
#include "mylib.h"

struct Queue * ready_q;
struct Queue * block_q;
ucontext_t *init_context;
struct _MyThread *running;
struct _MyThread *first_thread;



//Thread functions

void MyThreadInit(void(*start_funct)(void *), void *args){

	if(start_funct == NULL)
	{
		return;
	}

	else
	{

	struct _MyThread *new_thread;
	ucontext_t *new_context;
	char *pstack;
	Queue *child_q;
	pstack = (char *)malloc(8192*sizeof(char));
	if(pstack == NULL)
	{
		perror("Could not create stack\n");
		exit(-1);
	}

	new_context = (ucontext_t*)malloc(sizeof(ucontext_t));
	if(new_context == NULL)
	{
		perror("Could not create context\n");
		exit(-1);
	}
	init_context = (ucontext_t*)malloc(sizeof(ucontext_t));
	if(init_context == NULL)
	{
		perror("Could not create context\n");
		exit(-1);
	}
	
	new_thread = (struct _MyThread *)malloc(sizeof(struct _MyThread));
	if(new_thread == NULL)
	{
		perror("Could not create thread\n");
		exit(-1);
	}
	new_thread->parent = NULL;
	new_thread->blocked = NULL;

	//creating the ready and block queues
	ready_q = (struct Queue*)malloc(sizeof(struct Queue));
	if(ready_q == NULL)
	{
		perror("Could not create ready queue\n");
		exit(-1);
	}
	block_q = (struct Queue*)malloc(sizeof(struct Queue));
	if(block_q == NULL)
	{
		perror("Could not create block queue\n");
		exit(-1);
	}
	child_q = (struct Queue *)malloc(sizeof(struct Queue));
	if(child_q == NULL)
	{
		perror("Could not create child queue\n");
		exit(-1);
	}

	ready_q->head = NULL;
	ready_q->tail = NULL;
	block_q->head = NULL;
	block_q->tail = NULL;
	child_q->head = NULL;
	child_q->tail = NULL;

	getcontext(new_context);

	new_context->uc_link = NULL;
	new_context->uc_stack.ss_sp = pstack;
	new_context->uc_stack.ss_size = 8192;

	makecontext(new_context, (void (*)(void)) start_funct, 1, args);
	new_thread->children = child_q;
	new_thread->this_context = new_context;

	if(new_thread != NULL)
		running = new_thread;
	first_thread = running;
	swapcontext(init_context, new_context);
	return;
	}

}

MyThread MyThreadCreate(void(*start_funct)(void *), void *args){
	if(start_funct == NULL)
	{
		return;
	}

	else
	{

	struct _MyThread *new_thread;
	new_thread = (_MyThread*)malloc(sizeof(_MyThread));
	if(new_thread == NULL)
	{
		perror("Could not create thread\n");
		exit(-1);
	}
	ucontext_t *new_context;
	new_context= (ucontext_t*)malloc(sizeof(ucontext_t));
	if(new_context == NULL)
	{
		perror("Could not create context\n");
		exit(-1);
	}
	char *p_stack;
	p_stack = (char *)malloc(8192*sizeof(char));
	if(p_stack == NULL)
	{
		perror("Could not create stack\n");
		exit(-1);
	}

	Queue *child_q2 = (struct Queue *)malloc(sizeof(struct Queue));
	if(child_q2 == NULL)
	{
		perror("Could not create child queue\n");
		exit(-1);
	}
	child_q2->head = NULL;
	child_q2->tail = NULL;

	getcontext(new_context);

	new_context->uc_link = running->this_context;
	new_context->uc_stack.ss_sp = p_stack;
	new_context->uc_stack.ss_size = 8192;
	makecontext(new_context, (void (*)(void)) start_funct, 1, args);


	new_thread->parent = running;
	new_thread->children = child_q2;
	add_to_q(new_thread, running->children);
	new_thread->this_context = new_context;
	new_thread->blocked = NULL; 
	add_to_q(new_thread, ready_q);

	return (MyThread)(void *)new_thread;

	}


}


void MyThreadYield(void){
	if(is_q_empty(ready_q)==0){
		add_to_q(running, ready_q);
		struct _MyThread *old_thread;
		old_thread = running;
		running = rem_from_q(ready_q);
		swapcontext(old_thread->this_context, running->this_context);

	}

}


int MyThreadJoin(MyThread thread){

	if(thread == NULL)
	{
		perror("Null Thread passed to Join\n");
		return;
	}
	else
	{
		_MyThread *child = (_MyThread *)thread;

		if(has_thread(running->children, child) && child->parent == running){
			add_to_q(running, block_q);
			running->blocked = child;
			running = rem_from_q(ready_q);
			swapcontext(child->parent->this_context, running->this_context);
			return 0;
		}

		return -1;
	}
}

void MyThreadJoinAll(void){

	if(is_q_empty(running->children)==0){

		_MyThread *old_thread = running;
		if(has_thread(block_q, running)==0)
		{
		add_to_q(running, block_q);
		}

		if(is_q_empty(ready_q)==0)
		{
			running = rem_from_q(ready_q);
		}
		else 
		{
			running->this_context = init_context;
		}

		swapcontext(old_thread->this_context, running->this_context);

	}
}


void MyThreadExit(void){

	_MyThread *this = running;
	_MyThread *parent = running->parent;

	if(parent != NULL){
		remove_thread(running, parent->children);
		if(is_q_empty(block_q)==0){
			if(has_thread(block_q, parent)){
				if((q_size(parent->children)==0) || parent->blocked == this){
					remove_thread(parent, block_q);
					parent->blocked = NULL;
					add_to_q(parent, ready_q);
				}
			}
		}
	}
   
   if(running->children != NULL)
   {
		struct Node *g_child = running->children->head;
	
		if(g_child == NULL){
		//	printf("No child found\n");
		}

		while(g_child != NULL){

			if(running == first_thread)
				g_child->curr_thread->parent = NULL;
			else
			{

				g_child->curr_thread->parent = first_thread;
				add_to_q(g_child->curr_thread, first_thread->children);
			}

			g_child = g_child->next;
		}
	}
	free(this->children);
	free(this);

	if(is_q_empty(ready_q)==0){

		running = rem_from_q(ready_q);
	}
	else
	{
		running->this_context = init_context;
	}
	setcontext(running->this_context);
}


MySemaphore MySemaphoreInit(int initialValue){

	if(initialValue < 0){
		return;
	}
	else
	{

		struct _MySemaphore *semaphore;
		
		semaphore = (struct _MySemaphore *)malloc(sizeof(struct _MySemaphore));

		struct Queue *s_queue = (struct Queue *)malloc(sizeof(struct Queue));
		semaphore->value = initialValue;
		semaphore->sem_queue = s_queue;
		semaphore->sem_queue->head = NULL;
		semaphore->sem_queue->tail = NULL;

		return (MySemaphore)(void *)semaphore;
	}
}

void MySemaphoreSignal(MySemaphore sem){

	if(sem == NULL){
		perror("Null value provided to semaphore signal");
		return;
	}

	struct _MySemaphore *semaphore = (struct _MySemaphore *)sem;
	struct _MyThread *out_thread;

	semaphore->value = semaphore->value + 1;

	if(semaphore->value <= 0)
	{
		out_thread = rem_from_q(semaphore->sem_queue);
		add_to_q(out_thread, ready_q);
		return;
	}
}

void MySemaphoreWait(MySemaphore sem){

	if(sem == NULL){
		perror("Null value provided to semaphore wait");
		return;
	}
	else
	{
		struct _MySemaphore *semaphore = (struct _MySemaphore *)sem;
		struct _MyThread *in_thread;

		semaphore->value = semaphore->value - 1;
		
		if(semaphore->value < 0)
		{
			in_thread = running;
			add_to_q(in_thread, semaphore->sem_queue);

			//print_q(ready_q);
			if(is_q_empty(ready_q)==0)
			{
				//printf("wait:ready q not empty\n");
				running = rem_from_q(ready_q);
				swapcontext(in_thread->this_context, running->this_context);
			}
			else 
			{
				//printf("%u\n", init_context );
				running->this_context = init_context;
				//printf("ready q empty\n");
				setcontext(init_context);
			}

			
		}
	}
}

int MySemaphoreDestroy(MySemaphore sem){

	if(sem == NULL){
		perror("Null value provided to semaphore destroy");
		return;
	}
	else
	{
		struct _MySemaphore *semaphore = (struct _MySemaphore *)sem;

		if(is_q_empty(semaphore->sem_queue)==1){
			//printf("In destroy: if\n");
			free(semaphore->sem_queue);
			free(semaphore);
			return 0;
		}
		else
		{
			//printf("In destroy: else\n");
			return -1;
		}
	}
}


