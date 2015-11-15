// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 7
// Problem 1 -- Test the test-and-set
// gcc tas64.s testandset.c -o P1 -lrt
// testandset.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int tas(volatile char *lock);

main(int argc, char *argv[])
{
	int fd;
	unsigned long *sharedInt;
	pid_t pid;
	size_t size = sizeof(unsigned long)*2;
	int procNum = 0;
	int numCores = 4;
	int numIterations = 10000000;
	int temp; // used for incrementing the int in shared memory

	fd = shm_open("/myregion", O_RDWR | O_CREAT | O_TRUNC, 0766);
	if (fd < 0)
	{
		perror("Error creating shared memory object");
		exit(-1);
	}

	// Used to avoid bus error
	if(ftruncate(fd,size) < 0)
	{
		perror("Error truncating shared memory object");
		exit(-1);
	}

	sharedInt = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (sharedInt == MAP_FAILED)
	{
		perror("Error creating mmap");
		exit(-1);
	}
	*sharedInt = 0L;

	for(procNum = 0; procNum < numCores; procNum++)
	{
		pid=fork();
		if(pid == 0)
		{
			for(temp = 0; temp < numIterations; temp++)
				(*sharedInt)++;
			exit(0);
		}
	}
	while(wait(NULL) != -1);

	printf("First test complete.\n");
	printf("The value in memory should be %d.\n", numCores*numIterations);
	printf("The value we have is: %lu\n\n", *sharedInt);
	
	volatile char *lock = (char*) sharedInt+sizeof(unsigned long);
	*lock = 0;
	// Reset test...
	*sharedInt = 0L;

	for(procNum = 0; procNum < numCores; procNum++)
	{
		pid = fork();
		if (pid == 0)
		{
			for(temp = 0; temp < numIterations; temp++)
			{
				while(tas(lock));
				(*sharedInt)++;
				*lock = 0;
			}
			exit(0);
		}
	}
	while(wait(NULL) != -1);

	printf("Second test complete.\n");
	printf("The value in memory should be %d.\n", numCores*numIterations);
	printf("The value we have is: %lu\n", *sharedInt);

	return 0;
}