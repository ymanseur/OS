// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// fifo.h

#ifndef FIFO_H
#define FIFO_H

#include "sem.h"
#define MYFIFO_BUFSIZ 4096 /* Length of the FIFO */

struct fifo
{
	unsigned long buf[MYFIFO_BUFSIZ]; /* Data buffer */
	int front, back;				  /* Pointers to front and back of FIFO */
	struct sem mutex, read, write;	  /* Semaphores to use */
};

/* Initialize the shared memory FIFO *f */
void fifo_init(struct fifo *f);

/* Enque the data word d into the FIFO */
void fifo_wr(struct fifo *f, unsigned long d);

/* Deque the next data word from the FIFO and return it. */
unsigned long fifo_rd(struct fifo *f);

#endif