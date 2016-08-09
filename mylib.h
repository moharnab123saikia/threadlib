#ifndef MYLIB_H
#define MYLIB_H
#include <ucontext.h>
#include "queue.h"
#define STACK 16384 //16KB stack size

//Structure for thread
typedef struct _MyThread{
	struct _MyThread *parent;
	struct Queue *children;
	struct _MyThread *blocked;
	ucontext_t *this_context;
}_MyThread;

typedef struct _MySemaphore{
	int value;
	struct Queue *sem_queue;
}_MySemaphore;

#endif //MYLIB_H