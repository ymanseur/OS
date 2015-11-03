// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 5
// mmap.c

#include <stdlib.h>
#include <stdio.h>

// "When one has mapped a file for read-only access, but attempts
// to write to that mapped area, what signal is generated?"
void a()
{

}

// "If one maps a file with MAP_SHARED(part = 0) or MAP_PRIVATE(part = 1)
// and then writes to the mapped memory, is that update visible when
// accessing the file through the traditional lseek(2)/read(2) system calls?"
void bc(int part)
{

}

// "Say a pre-existing file of a certain size which is not an exact
// multiple of the page size is mapped with MAP_SHARED and read/write 
// access, and one writes to the mapped memory just beyond the byte
// corresponding to the last byte of the existing file. Does the size of the
// file through the traditional interface (e.g. stat(2)) change?"
// Explain your reasoning why this is or is not the case
void d()
{

}

// "Let us say that after performing the aformentioned memory write, one
// then increased the size of the file beyond the written area, without
// over-writing the intervening contents (e.g. by using lseek(2) and then
// write(2) with a size of 1), thus creating a 'hole' in the file. What
// happens to the data that had been written to this hole previously via
// the memory-map? Are they visible in the file?"
void e()
{

}

// Let's say there is an existing small file (say 10 bytes). Can you
// establish a valid mmap region two pages (8192 bytes) long? If so, what
// signal is delivered when attempting to access memory in the second page?
// What about the first page? Explain any differences in these outcomes.
void f()
{

}

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Error: Incorrect input\n");
		exit(1);
	}

	char question = *argv[1];

	switch(question)
	{
		case('A'):
			a();
			break;
		case('B'):
			bc(0);
			break;
		case('C'):
			bc(1);
			break;
		case('D'):
			d();
			break;
		case('E'):
			e();
			break;
		case('F'):
			f();
			break;
		default:
			fprintf(stderr, "Error: Incorrect argument.\nAccepted arguments (A-F)\n");
			exit(0);
	}
	exit(0);
}