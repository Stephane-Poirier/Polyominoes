#include "border_queue.hpp"
#include <memory>

border_queue::border_queue(const int size) : head(0), tail(0) {
	this->buffer_x =  (int *) malloc(sizeof(int) * size);
	this->buffer_y =  (int *) malloc(sizeof(int) * size);
	this->size = size;
}

border_queue::~border_queue() {
	free(this->buffer_x);
	free(this->buffer_y);
}

void border_queue::copy(border_queue *bq_src) {
	assert(this->size >= bq_src->size);

	this->head = bq_src->head;
	this->tail = bq_src->tail;

	memcpy(this->buffer_x, bq_src->buffer_x, sizeof(int)*bq_src->size);
	memcpy(this->buffer_y, bq_src->buffer_y, sizeof(int)*bq_src->size);

	return;
}

void border_queue::border_enqueue(int x, int y) {
	buffer_x[tail] = x;
	buffer_y[tail] = y;
	if (tail == size-1)
		tail = 0;
	else
		tail++;

	if (tail+1 == head) printf("queue is full\n");
	return;
}

bool border_queue::border_dequeue(int *x, int *y) {
	if (tail == head) return false;

	*x = buffer_x[head];
	*y = buffer_y[head];

	if (head == size-1)
		head = 0;
	else
		head++;

	return true;
}

void border_queue::print() {
	printf(" queue : head %d, tail %d\n", head, tail);
	if (head == tail) printf(". empty queue");
	else if (head < tail) {
		int i;
		for (i = head; i < tail; i++) printf("(%d, %d) ", buffer_x[i], buffer_y[i]);
	}
	else {
		int i;
		for (i = head; i < size; i++) printf("(%d, %d) ", buffer_x[i], buffer_y[i]);
		for (i = 0; i < tail; i++) printf("(%d, %d) ", buffer_x[i], buffer_y[i]);
	}
	printf("\n");
}