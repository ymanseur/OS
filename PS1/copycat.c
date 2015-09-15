// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 1

#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int c = -1;
	int bufferSize = 256;
	char* outfile = NULL;

	// Read Command Line Arguments
	while ((c=getopt(argc, argv, "b:o:")) != -1)
	{
		switch (c)
		{
			case 'b':
				bufferSize = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			// if getopt() does not recognize an option character
			case '?':
				if (optopt == 'b' || optopt == 'o')
					fprintf(stderr, "Usage: -%c incorrect/missing argument.\n", optopt);
				else
					fprintf(stderr, "Usage: %s [-b ###] [-o outfile] infile1 [...infile2....]\n", argv[0]);
				return -1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-b ###] [-o outfile] infile1 [...infile2....]\n", argv[0]);
				return-1;
		}
	}

	return 0;
}
