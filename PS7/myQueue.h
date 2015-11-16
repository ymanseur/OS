// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// myQueue.h
// Making this to help with testing of fifotest.c
#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdio.h>
#include <stdlib.h>


struct node
{
	int data;
	struct node* next;
};

struct queue
{
	struct node* front;
	struct node* back;
};

struct queue* qinit(void);

void enqueue(struct queue* q, int d);

int dequeue(struct queue* q);

#endif