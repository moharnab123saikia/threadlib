#include <stdio.h>
#include "mythread.h"
#include "queue.h"
#include "mythreadlib.h"
#include <ucontext.h>

void t0(void * dummy)
{
  printf("t0 start\n");
  MyThreadExit();
}

int main()
{
	MyThreadInit(t0, NULL);
	int s = sizeof(STACK);
  	printf("MyThread: %d", s);
}
