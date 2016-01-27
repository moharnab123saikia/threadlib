#include "queue.h"
#include "queue.c"
#include <assert.h>
#include <stdbool.h>
#include <ucontext.h>
#include "mythreadlib.h"


static void test_insert_and_deque_for_empty_queue() {
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	queue->head = NULL;
	queue->tail = NULL;

	_MyThread *thread2;
	has_thread(queue, thread2);
	_MyThread *thread1 = (_MyThread *)malloc(sizeof(_MyThread));	
	_MyThread *thread3 = (_MyThread *)malloc(sizeof(_MyThread));
	_MyThread *thread4 = (_MyThread *)malloc(sizeof(_MyThread));
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	getcontext(ctxt);
	thread1->this_context = ctxt;
	add_to_q(thread1, queue);
	assert(queue->head == queue->tail);
	assert(sizeOfQueue(queue)==1);
	assert(is_q_empty(queue)==0);
	add_to_q(thread2, queue);
	assert(has_thread(queue, thread2)==1);
	assert(queue->head->next == queue->tail);
	add_to_q(thread3, queue);
	assert(sizeOfQueue(queue)==3);
	add_to_q(thread4, queue);
	assert(sizeOfQueue(queue)==4);
	//assert(queue->tail == NULL);
	//rem_from_q(queue);
	assert(rem_from_q(queue)==thread1);
	assert(remove_thread(thread2, queue)==thread2);
	printf("\nQueue head %d", queue->tail);
	assert(sizeOfQueue(queue)==2);
	//assert(queue->tail==NULL);
}	

int main() {
	//test_initialize_queue();
	test_insert_and_deque_for_empty_queue();
	//test_remove_from_queue();
	return 0;
}
