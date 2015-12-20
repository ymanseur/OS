// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 9
// test.c

#include "sched.h"
#include <stdlib.h>
#include <stdio.h>

#define DELAY_FACTOR 29

void init_fn() 
{
	int i,x;
	for(i = 0; i < 30; i++)
	{
		switch(sched_fork())
		{
			case -1:
				perror("fork failed!!");
				exit(-1);
			case 0:
				fprintf(stderr,"<<in child addr %p>>\n",&x);
				child_fn();
				fprintf(stderr,"!!BUG!! at %s:%d\n",__FILE__,__LINE__);
				return;
			default:
				fprintf(stderr,"<<in parent addr %p>>\n",&x);
				break;
		}
	}
	sched_wait(&x);
	printf("init_fn Finished\n");
	exit(0);
}

child_fn()
{
	int y;
	int nice = 19 - (rand() % 40);
	sched_nice(nice);
	for(y=0;y<1<<DELAY_FACTOR;y++)
		;
	int pid = sched_getpid();
	sched_exit(pid);
}

main ()
{
	sched_init(init_fn);
	fprintf(stderr,"Whoops\n");
}