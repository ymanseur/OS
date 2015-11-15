// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// sem.h

#ifndef SEM_H
#define SEM_H

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#define N_PROC 64 /* Maximum number of virtual processors required to accept */

int my_procnum; /* small integer identifier */

struct sem
{
	volatile char lock;	  /* lock */
	int semval;			  /* semaphore value */
	int semwait[N_PROC];  /* waiting */
	pid_t sempid[N_PROC]; /* ID of process that did last op */
};

int tas(volatile char *lock);

// Initialize the semaphore *s with the initial count.
void sem_init(struct sem *s, int count);

// Attempt to atomically decrement the semaphore
int sem_try(struct sem *s);

// Atomically decrement the semaphore, blocking until successful.
void sem_wait(struct sem *s);

// Wake other processes
void sem_inc(struct sem *s);

#endif