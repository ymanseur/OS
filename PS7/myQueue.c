// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// myQueue.c
// Making this to help with testing of fifotest.c

#include "myQueue.h"

struct queue* qinit(void)
{
	struct queue* p = malloc(1*sizeof(*p));

	if(p==NULL)
		exit(-1);
	p->front = NULL;
	p->back = NULL;
	return p;
}

void enqueue(struct queue* q, int d)
{
	struct node* p = malloc(1*sizeof(p));
	if(p==NULL)
		return;
		
	p->data = d;
	p->next = NULL;

	if(q==NULL)
	{
		free(p);
		return;
	}
	else if(q->front == NULL && q->back == NULL)
	{
		q->front = p;
		q->back = p;
		printf("FIRSTCOMMIT\n");
		return;
	}
	else if(q->front == NULL || q->back == NULL)
	{
		free(p);
		return;
	}
	else
	{
		q->back->next = p;
		q->back = p;
	}
}

int dequeue(struct queue* q)
{
	struct node* f = NULL;
	struct node* p = NULL;

	if(q==NULL)
		return -1;
	else if(q->front == NULL && q->back == NULL)
		return -1;
	else if(q->front == NULL || q->back == NULL)
		return -1;
	f = q->front;
	p = f->next;
	free(f);
	q->front = p;
	if(q->front == NULL)
		q->back == q->front;
	return q->front->data;
}