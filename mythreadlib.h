#ifndef MYTHREADLIB_H
#define MYTHREADLIB_H
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

#endif //MYTHREADLIB_H