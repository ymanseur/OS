// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// fifotest.c

/*
	Instructions:
	1. Establish a struct fifo in shared memory
	2. Create two virtual processors, one of which will be the writer and the other the reader
	3. Have the writer send a fixed number of sequentially-numbered data using fifo_wr
	4. Have the reader read these and verify that all were received
	5. Next, create multiple writers but one reader
		a. In a successful test, all of the writers' streams will be received by the reader
		   complete, in (relative) sequence, with no missing or duplicated items, and all 
		   processes will eventually run to completion and exit (no hanging).
		b. A suggested approach is to treat each datum (32-bit word) as a bitwise word
		   consisting of an ID for the writer and the sequence number
		c. It is not necessary to test under multiple readers, but your fifo code should work
		   correctly for this case
	
	Notes:
	1. Use reasonable test parameters
	2. Remember for the acid test to make the buffer fill and empty quite a few times
	3. You should be able to demonstrate failure by deliberately breaking something in
	   your implementation (reversing the order of two operations)
	4. Then demonstrate success under a variety of strenuous conditions
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "sem.h"
#include "fifo.h"

struct fifo *f;

void reportError(char *e)
{
	perror(e);
	exit(-1);
}

void createMultipleWriters(int numProc, int numWords)
{
	FILE *written;
	written = fopen("written.txt", "a");
	if(written==NULL)
		reportError("Error opening temp file");
	int ii, jj;
	pid_t pid;
	for(ii = 0; ii < numProc; ii++)
	{
		switch(pid=fork())
		{
			case -1: 
				reportError("Error creating fork");
			case 0:
				my_procnum = ii+1;
				for(jj = ii*numWords; jj < (ii+1)*numWords; jj++)
				{
					fifo_wr(f,jj);
					printf("Process %d wrote %d\n", my_procnum, jj);
					fprintf(written, "%d\n", jj);
				}
				exit(0);
		}
	}
	while(wait(NULL) != -1);
	fclose(written);
}

void createSingleReader(int numProc, int numWords)
{
	FILE *read;
	read = fopen("read.txt", "a");
	int jj, total = 0;
	pid_t pid;
	unsigned long value;

	switch(pid=fork())
	{
		case -1:
			reportError("Error creating fork");
		case 0:
			for(jj = 0; jj < numProc*numWords; jj++)
			{
				value = fifo_rd(f);
				total++;
				printf("Value #%d read: %lu\n", total, value);
				fprintf(read, "%lu\n", value);
			}
			printf("Total words received: %d\n", total);
			exit(0);
	}
	while(wait(NULL) != -1);
	fclose(read);
}

void compareResults()
{
	FILE *f1, *f2;
	int c1, c2;
	int totalErrors=0;
	f1 = fopen("written.txt", "r");
	f2 = fopen("read.txt", "r");
	c1 = fgetc(f1);
	c2 = fgetc(f2);
	while(c1!=EOF || c2 != EOF)
	{
		if(c1 != '\n' && c2 != '\n'){
			if(abs(c1-c2) > 0)
				totalErrors ++;
			printf("%d\n", abs(c1-c2));
			//printf("%c, %c\n", c1, c1);
		}
			
		c1 = fgetc(f1);
		c2 = fgetc(f2);
	}
	printf("Total Errors found: %d\n", totalErrors);
	fclose(f1);
	fclose(f2);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		reportError("Please enter only two arguments");

	int numProc = atoi(argv[1]);
	int numWords = atoi(argv[2]);

	if(numProc > 64)
		reportError("Error: Maximum number of virtual processors exceeded");

	f = mmap(NULL, sizeof(struct fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(f == MAP_FAILED)
		reportError("Error creating mmap");

	fifo_init(f);
	my_procnum = 0;

	createMultipleWriters(numProc, numWords);
	createSingleReader(numProc, numWords);
	compareResults();

	remove("written.txt");
	remove("read.txt");
	
	return 0;	
}
