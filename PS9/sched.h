// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 9
// sched.h

#ifndef SCHED_H
#define SCHED_H

#define SCHED_NPROC 256
#define SCHED_READY 0
#define SCHED_RUNNING 1
#define SCHED_SLEEPING 2
#define SCHED_ZOMBIE 4
#define STACK_SIZE 65536

#include "savectx64.h"

/* use this for each simulated task */
/* internals are up to you */
/* probably should include things like the task state */
/* priority, accumulated, cpu time, stack address, etc. */
struct sched_proc {
	int state;			/* Task State */
	int nice;			/* Nice Value */
	int priority; 		/* Static Priority */
	int accumulated;	/* Accumulated Processing Time */
	int cpuTime;		/* CPU Time */
	void *stack;		/* Stack address */
	int pid;			/* Process ID */
	int ppid;			/* Parent Process ID */
	int code;			/* Exit code */
	int children;		/* Number of Living Children */
	struct savectx ctx;	/* For restoring context */
};

/* use this for each event/wakeup queue */
/* internals are up to you */
struct sched_waitq {
	struct sched_proc *procs[SCHED_NPROC];
};

/*
	This function will be called once by the testbed program.
	It should initialize your scheduling system, including
	setting up a periodic interval timer (see setitimer),
	establishing sched_tick as the signal handler for that timer,
	and creating the initial task which will have pid of 1. After
	doing so, make pid 1 runnable and transfer execution to it
	(including switching to its stack) at location init_fn. This
	function is not expected to return and if it does so it is OK
	to have unpredictable results.
*/
void sched_init(void (*init_fn)());

/*
	Just like the real fork, create a new simulated task which
	is a copy of the caller. Allocate a new pid for the child, 
	and make the child runnable and eligible to be scheduled.
	sched_fork returns 0 to the child and the child's pid to the
	parent. It is not required that the relative order of parent
	vs child being scheduled be defined. On error, return -1.

	Unlike the real fork, you do not need to duplicate the entire
	address space. Parent and child will execute in the same
	address space. However, you will need to create a new private
	stack area for the child and initialize it to be a copy of 
	the parent's. See below for discussion on stacks.
*/
int sched_fork();

/*
	Terminate the current task, making it a ZOMBIE, and store
	the exit code. If a parent is sleeping in sched_wait(), 
	wake it up and return the exit code to it. There will be 
	no equivalent of SIGCHILD. sched_exit will not return. 
	Another runnable process will be scheduled.
*/
void sched_exit(int code);

/*
	Return the exit code of a zombie child and free the 
	resources of that child. If there is more than one
	such child, the order in which the codes are returned
	is not defined. If there are no zombie children, but 
	the caller does have at least one child, place the 
	caller in SLEEPING, to be woken up when a child calls
	sched_exit(). If there are no children, return
	immediately with -1, otherwise the return value is 0.
	Since there are no simulated signals, the exit code
	is simply the integer from sched_exit().
*/
int sched_wait(int *exit_code);

/*
	Set the current task's "nice value" to the supplied parameter.
	Nice values may range from +19 (least preferred static
	priority) to -20 (most preferred). Clamp any out-of-range
	values to those limits
*/
void sched_nice(int niceval);

/* 	return current task's pid */
int sched_getpid();

/*	return pid of the parent of the current task */
int sched_getppid();

/*	return number of timer ticks since startup */
int sched_gettick();

/*
	output to stdout a listing of all of the current tasks,
	including sleeping and zombie tasks. List the following
	information in tabular form:
		pid
		ppid
		current state
		base addr of private stack area
		static priority
		dynamic priority
		total CPU time used (in ticks)
	You should establish sched_ps() as the signal handler 
	for SIGABRT so that a ps can be forced at any time
	by sending the testbed SIGABRT
*/
void sched_ps();

/*
	This is the suggested name of a required routine which will
	never be called directly by the testbed. sched_switch()
	should be the sole place where a context switch is made,
	analogous to schedule() within the Linux kernel.
	sched_switch() should place the current task on the run queue
	(assuming it is READY), then select the best READY task from
	the runqueue, taking into account the dynamic priority of each
	task. The selected task should then be placed in the RUNNING
	state and a context switch made it it (unless, of course, the
	best task is also the current task) See discussion below on
	support routines for context switch.
*/
void sched_switch();

/*
	This is the suggested name of a required routine which will
	never be called directly by the testbed, but instead will be
	the signal handler for the SIGVTALRM signal generated by the
	periodic timer. Each occurrence of the timer signal is
	considered a tick. The number of ticks since sched_init is to
	be returned by sched_gettick().
	sched_tick should examine the currently running task and if its
	time slice has just expired, mark that task as READY, place it
	on the run queue based on its current dynamic priority, and
	then call sched_switch() to cause a new task to be run. Watch 
	out for signal mask issues...remember SIGVTALARM will, by 
	default, be masked on entry to your signal handler.
*/
void sched_tick();

#endif