// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// fifo.c

#include "fifo.h"

void fifo_init(struct fifo *f)
{
	f->front = 0;
	f->back = 0;
	sem_init(&(f->mutex), 1);
	sem_init(&(f->read), 0); /* Because FIFO is empty */
	sem_init(&(f->write), MYFIFO_BUFSIZ); /* Because FIFO is empty */
}

void fifo_wr(struct fifo *f, unsigned long d)
{
	sem_wait(&(f->write)); /* Block until FIFO has room */
	sem_wait(&(f->mutex));
	f->buf[f->back] = d; /* Insert d at the back of the FIFO */
	f->back = (f->back + 1) % MYFIFO_BUFSIZ; /* Increment index value */
	sem_inc(&(f->read)); /* Wake the rest */
	sem_inc(&(f->mutex));
}

unsigned long fifo_rd(struct fifo *f)
{
	sem_wait(&(f->read)); /* Block until there are words queued in the FIFO */
	sem_wait(&(f->mutex));
	unsigned long d = f->buf[f->front]; 
	f->front = (f->front + 1) % MYFIFO_BUFSIZ; /* Increment index value */
	sem_inc(&(f->write)); /* Wake the rest */
	sem_inc(&(f->mutex));
	return d;
}