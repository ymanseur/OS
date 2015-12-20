// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 9
// sched.c

#include "sched.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>

// Global Variables
struct sched_proc *currentProc;
struct sched_waitq *q;
int numTicks;
int NEED_RESCHED;
sigset_t mask;

void sched_init(void (*init_fn)())
{
	void *newsp;	/* Stack Area */
	struct itimerval timer;
	struct sigaction sa;
	NEED_RESCHED = 0;

	// Initialize the periodic interval timer to tick every 100ms
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 100000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 100000;

	// Set periodic interval timer
	if((setitimer(ITIMER_VIRTUAL, &timer, NULL)) < 0)
	{
		perror("setitimer failed");
		exit(-1);
	}

	sigemptyset(&mask);
	sigaddset(&mask,SIGVTALRM);

	sa.sa_flags=0;
	sa.sa_handler = &sched_tick;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGVTALRM,&sa,NULL);
	sa.sa_handler=sched_ps;
	sigaction(SIGABRT,&sa,NULL);

	// Initialize stack area
	if ((newsp = mmap(0, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
	{
		perror("mmap failed");
		exit(-1);
	}

	// Malloc first process
	if((currentProc = malloc(sizeof(struct sched_proc))) < 0)
	{
		perror("malloc failed in sched_init");
		exit(-1);
	}

	// Malloc Process Queue
	if((q = malloc(sizeof(struct sched_waitq))) < 0)
	{
		perror("malloc failed in sched_init");
		exit(-1);
	}

	// Initialize task info
	currentProc->state = SCHED_RUNNING;
	currentProc->stack = newsp;
	currentProc->pid = 1;
	currentProc->ppid = 0;
	currentProc->accumulated = 0;
	currentProc->cpuTime = 0;
	currentProc->nice = 0;
	savectx(&(currentProc->ctx));
	currentProc->ctx.regs[JB_BP] = currentProc->stack + STACK_SIZE;
	currentProc->ctx.regs[JB_SP] = currentProc->stack + STACK_SIZE;
	currentProc->ctx.regs[JB_PC] = init_fn;

	q->procs[1] = currentProc;
	restorectx(&(currentProc->ctx),0);
}

int sched_fork()
{
	// Block Signals
	sigprocmask(SIG_BLOCK,&mask,NULL);

	void *newsp;
	struct sched_proc *child;
	// Assign new pid
	int i, newPid = -1;
	for(i = 1; i < SCHED_NPROC; i++)
	{
		if(!q->procs[i])
		{
			newPid = i;
			break;
		}
	}

	if(newPid == -1)
	{
		fprintf(stderr, "Maximum Number Of Processes Reached\n");
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		return -1;
	}

	if((child = malloc(sizeof(struct sched_proc))) < 0)
	{
		perror("malloc failed in fork");
		exit(-1);
	}

	// Initialize stack area
	if ((newsp = mmap(0, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
	{
		perror("mmap failed");
		exit(-1);
	}

	//Assign values
	child->pid = newPid;
	child->ppid = currentProc->pid;
	child->state = SCHED_READY;
	child->accumulated = 0;
	child->stack = newsp;
	child->nice = currentProc->nice;
	child->cpuTime = currentProc->cpuTime;
	child->accumulated = 0;

	q->procs[newPid] = child;
	if(!savectx(&child->ctx))
	{
		memcpy(child->stack, currentProc->stack, STACK_SIZE);
		adjstack(child->stack, child->stack + STACK_SIZE, child->stack - currentProc->stack);
		child->ctx.regs[JB_BP] += child->stack - currentProc->stack;
		child->ctx.regs[JB_SP] += child->stack - currentProc->stack;
	}
	else
	{
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		return 0;
	}
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
	return child->pid;
}

void sched_exit(int code)
{
	// Block Signals
	sigprocmask(SIG_BLOCK,&mask,NULL);

	currentProc->state = SCHED_ZOMBIE;
	currentProc->code = code;
	printf("Process %d exited with code %d\n", currentProc->pid, code);

	// wake up parent
	int i;
	for(i = 0; i < SCHED_NPROC; i++)
	{
		if(q->procs[i] && q->procs[i]->pid == currentProc->ppid && q->procs[i]->state == SCHED_SLEEPING)
		{
			printf("Process %d is now waking up\n", q->procs[i]->pid);
			q->procs[i]->state = SCHED_READY;
			break;
		}
	}
	sched_switch();
}

int sched_wait(int *exit_code)
{
	// Block Signals
	sigprocmask(SIG_BLOCK,&mask,NULL);

	int i;
	int children = 0;
	for(i = 0; i < SCHED_NPROC; i++)
		if(q->procs[i])
			if(q->procs[i]->ppid == currentProc->pid)
				children++;

	if(children == 0)
		return -1;

	// Check for zombie children
	for(i=0; i< SCHED_NPROC; i++)
	{
		if(q->procs[i] && q->procs[i]->ppid == currentProc->pid)
		{
			if (q->procs[i]->state == SCHED_ZOMBIE)
			{
				printf("Zombie Process %d killed by Parent Process %d.\n", q->procs[i]->pid, currentProc->pid);
				*exit_code = q->procs[i]->code;
				if(munmap(q->procs[i]->stack, STACK_SIZE) < 0)
				{
					perror("munmap failed in sched_wait");
					exit(-1);
				}

				free(q->procs[i]);
				q->procs[i] = NULL;
				sched_switch();
			}
			else
			{
				currentProc->state = SCHED_SLEEPING;
				sigprocmask(SIG_BLOCK,&mask,NULL);
				sched_switch();
				sigprocmask(SIG_UNBLOCK,&mask,NULL);
				// Woken up because child died
				printf("Zombie Process %d killed by Parent Process %d.\n", q->procs[i]->pid, currentProc->pid);
				*exit_code = q->procs[i]->code;
				if(munmap(q->procs[i]->stack, STACK_SIZE) < 0)
				{
					perror("munmap failed in sched_wait");
					exit(-1);
				}
				free(q->procs[i]);
				q->procs[i] = NULL;
			}
		}
	}
	for(i = 0; i < SCHED_NPROC; i++)
		if(q->procs[i])
			if(q->procs[i]->ppid == currentProc->pid)
				children++;

	if(children == 0)
		return -1;
	return 0;
}

void sched_nice(int niceval)
{
	if(niceval >= 19)
		currentProc->nice = 19;
	else if(niceval <=-20)
		currentProc->nice = -20;
	else
		currentProc->nice = niceval;
}

int sched_getpid()
{
	return currentProc->pid;
}

int sched_getppid()
{
	return currentProc->ppid;
}

int sched_gettick()
{
	return numTicks;
}

void sched_ps()
{
	printf("pid\tppid\tcurrent state\tbase addr\tnice\tstatic\ttotal ticks\n");
	int i;
	for(i = 1; i < SCHED_NPROC; i++)
	{
		if(q->procs[i])
		{
			printf("%d\t", q->procs[i]->pid);
			printf("%d\t", q->procs[i]->ppid);
			switch(q->procs[i]->state)
			{
				case SCHED_READY:   printf("READY   \t");break;
				case SCHED_RUNNING: printf("RUNNING \t");break;
				case SCHED_SLEEPING:printf("SLEEPING\t");break;
				case SCHED_ZOMBIE:  printf("ZOMBIE  \t");break;
			}
			printf("%p\t", q->procs[i]->stack);
			printf("%d\t", q->procs[i]->nice);
			printf("%d\t", q->procs[i]->priority);
			printf("%d\n", q->procs[i]->accumulated);
		}
	}
}

void sched_switch()
{
	if(NEED_RESCHED)
		NEED_RESCHED = 0;

	// Update priorities
	int i;
	for(i = 0; i < SCHED_NPROC; i++)
		if(q->procs[i])
		{
			q->procs[i]->priority = (20 + q->procs[i]->nice) + (10 * q->procs[i]->accumulated / (20 - q->procs[i]->nice));
			if (q->procs[i]->priority > 39)
			{
				q->procs[i]->priority = 39;
			}
		}

	if((currentProc->state != SCHED_SLEEPING) && (currentProc->state != SCHED_ZOMBIE))
	{
		currentProc->state = SCHED_READY;
	}

	int bestPriority = 39, bestIndex = -1;
	// Find best new process
	for(i = 0; i <SCHED_NPROC; i++)
		if(q->procs[i] && q->procs[i]->state == SCHED_READY)
			if(q->procs[i]->priority < bestPriority)
			{
				bestPriority = q->procs[i]->priority;
				bestIndex = i;
			}
	if(bestIndex == -1)
	{
		sched_ps();
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		return;
	}
	struct sched_proc *bestProc = q->procs[bestIndex];
	if(bestProc->pid == currentProc->pid)
	{
		currentProc->cpuTime = 0;
		currentProc->state = SCHED_READY;
		sched_ps();
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		return;
	}

	if(savectx(&(currentProc->ctx)) == 0)
	{
		currentProc = bestProc;
		currentProc->cpuTime = 0;
		currentProc->state = SCHED_RUNNING;
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		restorectx(&(currentProc->ctx),1);
	}
	sched_ps();
	return;
}

void sched_tick()
{
	sigprocmask(SIG_BLOCK,&mask,NULL);
	int window = 0;

	// Determine number of processes
	int i, numProcs = 0;
	for(i = 0; i < SCHED_NPROC; i++)
		if(q->procs[i] && q->procs[i]->state != SCHED_ZOMBIE)
			numProcs++;

	if(numProcs != 0)
		window = (int)(4/numProcs);

	numTicks++;
	currentProc->accumulated++;
	currentProc->cpuTime++;

	//if(currentProc->cpuTime > window){
		NEED_RESCHED = 1;
		sigprocmask(SIG_UNBLOCK,&mask,NULL);
		sched_switch();
	//}
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
}
