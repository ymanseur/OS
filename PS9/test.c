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
	int x;
	fprintf(stderr,"Hooray made it to init_fn, stkaddr %p\n",&x);
	switch(sched_fork())
	{
		case -1:
			perror("fork failed!!");
			exit(-1);
		case 0:
			fprintf(stderr,"<<in child addr %p>>\n",&x);
			child_fn1();
			fprintf(stderr,"!!BUG!! at %s:%d\n",__FILE__,__LINE__);
			return;
		default:
			fprintf(stderr,"<<in parent addr %p>>\n",&x);
			parent_fn();
			break;
	}
	printf("init_fn Finished\n");
	exit(0);
}

parent_fn()
{
	int y,p;
	fprintf(stderr,"Wow, made it to parent, stkaddr=%p\n",&y);
	switch(sched_fork())
	{
		case -1:
			perror("fork failed!!");
			exit(-1);
		case 0:
			child_fn2();
			sched_exit(11);
			fprintf(stderr,"!!BUG!! at %s:%d\n",__FILE__,__LINE__);
			return;
		default:
			sched_wait(&y);
			return;
	}
}

child_fn1()
{
	int y;
	fprintf(stderr,"CHILD 1: START &y=%p\n",&y);
	for(y=0;y<1<<DELAY_FACTOR;y++)
		;
	for(y=0;y<1<<DELAY_FACTOR;y++)
		;
	fprintf(stderr,"CHILD 1: DONE y=%d\n",y);
	sched_exit(22);
}

child_fn2()
{
	int y;
	sched_nice(4);
	fprintf(stderr,"CHILD 2: START &y=%p\n",&y);
	for(y=0;y<1<<DELAY_FACTOR;y++)
		;
	for(y=0;y<1<<DELAY_FACTOR;y++)
		;
	fprintf(stderr,"CHILD 2: DONE y=%d\n",y);
}

main ()
{
	sched_init(init_fn);
	fprintf(stderr,"Whoops\n");
}