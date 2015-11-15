// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// sem.c

#include "sem.h"

// Signal Handler{
void sig_handler(int signo){}

void sem_init(struct sem *s, int count)
{
	s->lock = 0;
	s->semval = count;
}

int sem_try(struct sem *s)
{
	while(tas(&(s->lock)) != 0);

	if (s->semval > 0)
	{
		(s->semval)--;
		s->lock = 0;
		return 1;
	}
	else
	{
		s->lock = 0;
		return 0;
	}
}

void sem_wait(struct sem *s)
{
	while(1)
	{
		while(tas(&(s->lock)) != 0);

		if(s->semval > 0)
		{
			(s->semval)--;
			s->semwait[my_procnum] = 0;
			s->lock = 0;
			return;
		}
		else
		{
			sigset_t mask;
			s->semwait[my_procnum] = 1;
			s->sempid[my_procnum] = getpid();
			sigfillset(&mask);
			sigdelset(&mask, SIGUSR1);
			sigdelset(&mask, SIGINT);
			sigprocmask(SIG_BLOCK, &mask, NULL);
			signal(SIGUSR1, sig_handler);
			s->lock = 0;
			s->semwait[my_procnum] = 1;
			sigsuspend(&mask);
			sigprocmask(SIG_UNBLOCK, &mask, NULL);
		}
	}
}

void sem_inc(struct sem *s)
{
	int id;
	while(tas(&(s->lock)) != 0);
	s->semval++;
	for(id = 0; id < N_PROC; id++)
	{
		if(s->semwait[id])
		{
			s->semwait[id] = 0;
			kill(s->sempid[id],SIGUSR1);
			break;
		}
	}
	s->lock = 0;
}

