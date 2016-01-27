#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include "mythread.h"
#include "queue.h"
#include "mythreadlib.h"

struct Queue * ready_q;
struct Queue * block_q;
ucontext_t *init_context;
struct _MyThread *running;
struct _MyThread *first_thread;



//Thread functions

void MyThreadInit(void(*start_funct)(void *), void *args){

	struct _MyThread *new_thread = (struct _MyThread *)malloc(sizeof(struct _MyThread));
	init_context = (ucontext_t*)malloc(sizeof(ucontext_t));
	ucontext_t *new_context = (ucontext_t*)malloc(sizeof(ucontext_t));
	char *pstack = (char *)malloc(8192*sizeof(char));

	if(pstack == NULL)
	{
		perror("ERROR:Not enough memory to create stack\n");
		exit(-1);
	}
	if(new_thread == NULL)
	{
		perror("ERROR:Not enough memory to create thread\n");
		exit(-1);
	}
	if(init_context == NULL)
	{
		perror("ERROR:Not enough memory to create stack\n");
		exit(-1);
	}

	//creating the ready and block queues
	ready_q = (struct Queue*)malloc(sizeof(struct Queue));
	block_q = (struct Queue*)malloc(sizeof(struct Queue));
	Queue *child_q = (struct Queue *)malloc(sizeof(struct Queue));

	ready_q->head = NULL;
	ready_q->tail = NULL;
	block_q->head = NULL;
	block_q->tail = NULL;
	child_q->head = NULL;
	child_q->tail = NULL;

	getcontext(new_context);

	new_context->uc_link = NULL;//running->this_context;
	new_context->uc_stack.ss_sp = pstack;
	new_context->uc_stack.ss_size = 8192;

	makecontext(new_context, (void (*)(void)) start_funct, 1, args);
	new_thread->parent = NULL;
	new_thread->blocked = NULL;
	new_thread->children = child_q;
	new_thread->this_context = new_context;
	running = new_thread;
	first_thread = running;
	//add_to_q(new_thread, ready_q);
	swapcontext(init_context, new_context);
	//printf("\nReturned to init");

	return;


}

MyThread MyThreadCreate(void(*start_funct)(void *), void *args){
	//printf("\ncraete: Line 75");
	struct _MyThread *new_thread = (_MyThread*)malloc(sizeof(_MyThread));
	//printf("\ncreate: thread created");
	ucontext_t *new_context = (ucontext_t*)malloc(sizeof(ucontext_t));
	//printf("\ncreate: context created");
	char *p_stack = (char *)malloc(8192*sizeof(char));
	Queue *child_q2 = (struct Queue *)malloc(sizeof(struct Queue));
	child_q2->head = NULL;
	child_q2->tail = NULL;

	getcontext(new_context);
	//printf("\ncreate: get context called");

	new_context->uc_link = running->this_context;
	//printf("\ncreate: context linked");
	new_context->uc_stack.ss_sp = p_stack;
	new_context->uc_stack.ss_size = 8192;
	//printf("\ncraete: Line 84 thread %d");
	makecontext(new_context, (void (*)(void)) start_funct, 1, args);
	new_thread->parent = running;
	new_thread->blocked = NULL; 
	new_thread->children = child_q2;
	add_to_q(new_thread, running->children);
	new_thread->this_context = new_context;
	//printf("\ncraete: Line 90 thread %u", new_thread);
	add_to_q(new_thread, ready_q);
    //printf("\nCREATE: Line 92");
	return (MyThread)(void *)new_thread;


}


void MyThreadYield(void){
	if(!is_q_empty(ready_q)){
		add_to_q(running, ready_q);
		struct _MyThread *old_thread;
		old_thread = running;
		running = rem_from_q(ready_q);
		swapcontext(old_thread->this_context, running->this_context);

	}

}


int MyThreadJoin(MyThread thread){
	_MyThread *child = (_MyThread *)thread;
	//printf("\nIn join");
	if(has_thread(running->children, child) && child->parent == running){
		add_to_q(running, block_q);
		running->blocked = child;
		running = rem_from_q(ready_q);
		swapcontext(child->parent->this_context, running->this_context);
		return 0;
	}

	return -1;
}

void MyThreadJoinAll(void){
	//printf("JOINAll: Line 125");

	if(is_q_empty(running->children)==0){
		//printf("JOINAll: Line 127");

		_MyThread *old_thread = running;
		if(has_thread(block_q, running)==0){
		add_to_q(running, block_q);
		//printf("Block q :\n");
		//print_q(block_q);
		}
		printf("\n Size : %d",sizeOfQueue(block_q));
		if(is_q_empty(ready_q)==0){
			//printf("Ready Q\n");
			//print_q(ready_q);
			running = rem_from_q(ready_q);
		}
		else 
		{
			//printf("JOINAll: Line 141\n");
			running->this_context = init_context;
		}


		swapcontext(old_thread->this_context, running->this_context);

	}
}


void MyThreadExit(void){

	//printf("Iam at start of exit\n");

	//printf("thread address: %u\n",running);


	_MyThread *this = running;

	_MyThread *parent = running->parent;

	if(parent != NULL){
		//printf("Parent:  %u ", parent);
		//printf("Running:  %u ", running);
		remove_thread(running, parent->children);
		//printf("\nparent->children");
		//print_q(parent->children);
		//printf("Empty BQ? %d", is_q_empty(block_q));
		//printf("Has Thred  %u : %d\n", parent, has_thread(block_q, parent));
		//printf("Size of Queue : %d\n", sizeOfQueue(parent->children));
	if(is_q_empty(block_q)==0){
		if(has_thread(block_q, parent)){
		//printf("Exit: Line 169  \n ");

		if((sizeOfQueue(parent->children)==0) || parent->blocked == this){
	//		printf("Exit: Line 172  \n ");
			//printf("\nBlock q before:\n");
			//print_q(block_q);
			remove_thread(parent, block_q);
			//printf("\nBlock q after:\n");
			//print_q(block_q);
			parent->blocked = NULL;
			add_to_q(parent, ready_q);
			//printf("\nReady q :\n");
			//print_q(ready_q);
		}
	}
}
}
   //printf("Exit: Line 179  \n ");
   if(running->children != NULL)
   {
	struct Node *g_child = running->children->head;
	if(g_child == NULL){
	//	printf("No child found\n");
	}

	while(g_child != NULL){

		if(running == first_thread)
			g_child->curr_thread->parent = NULL;
		else{

			g_child->curr_thread->parent = first_thread;
			add_to_q(g_child->curr_thread, first_thread->children);
		}

		g_child = g_child->next;
	}
}

	if(is_q_empty(ready_q)==0){

		running = rem_from_q(ready_q);
		//printf("ready q not empty\n");
		//setcontext(running->this_context);

	}
	else
	{
		running->this_context = init_context;
		//printf("No parent found\n");
		//setcontext(init_context);
	}
	//free(this->children);
	//free(this);
	setcontext(running->this_context);
}


