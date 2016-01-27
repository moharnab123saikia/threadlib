/******************************************************************************
 *
 *  File Name........: fib.c
 *
 *  Description......:
 *
 *  Created by vin on 11/21/06
 *
 *  No warranty!  This program has not been compiled or tested because
 *  I do not have a library with which to do so.
 *
 *  It is provided for illusration.  
 *
 *****************************************************************************/

#include "mythread.h"
 #include "queue.h"
#include "mythreadlib.h"
#include <ucontext.h>

// evaluate a Fibonacci number:
//	fib(0) = 0
//	fib(1) = 1
//	fib(n) = fib(n-1) + fib(n-2)  [n>1]
// this function is messy because we have to pass everything as a
// generic parameter (void*).
// also, the function parameter is a value/result -- therefore it is a
// pointer to an integer.
//
void fib(void *in)
{
  int *n = (int *)in;	 	/* cast input parameter to an int * */

  if (*n == 0)

    /* pass */printf("\nn=0");			/* return 0; it already is zero */

  else if (*n == 1)
    printf("\nn=1");
    /* pass */			/* return 1; it already is one */

  else {
    int n1 = *n - 1;
    printf("n1: %d\n", n1);		/* child 1 param */
    int n2 = *n - 2;		/* child 2 param */
    printf("n2: %d\n", n2);
    // create children; parameter points to int that is initialized.
    // this is the location they will write to as well.
    //printf("Before threadcreate \n");
    MyThreadCreate(fib, (void*)&n1);
    //printf("After threadcreate %d\n", n1);
    MyThreadCreate(fib, (void*)&n2);
    //printf("After threadcreate %d \n", n2);
    // after creating children, wait for them to finish
    //printf("Before joinall \n");
    MyThreadJoinAll();
    //printf("After join all \n");

    //  write to addr n_ptr points; return results in addr pointed to
    //  by input parameter
    *n = n1 + n2;
    //printf("n: %d\n", *n);
  }

  //printf("In exit \n");
  MyThreadExit();		// always call this at end
}

main(int argc, char *argv[])
{
  int n;

  if (argc != 2) {
    printf("usage: %s <n>\n", argv[0]);
    exit(-1);
  }
  n = atoi(argv[1]);
  if (n < 0 || n > 10) {
    printf("invalid value for n (%d)\n", n);
    exit(-1);
  }

  printf("fib(%d) = ", n);
  //printf("Inside main: before Init thread\n");
  MyThreadInit(fib, (void*)&n);
  printf("%d\n", n);
}


/*........................ end of fib.c .....................................*/
