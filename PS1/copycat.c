// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 1

#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

int processFiles(int iFD, int oFD, int bufferSize)
{
	char *buffer = malloc (bufferSize);
	int bytesRead = 0, bytesWrite = 0;
	if (buffer == NULL)
	{
		fprintf(stderr, "Cannot allocate buffer with size: %d\n", bufferSize);
		return -1;
	}

	// Loop until end of file
	while ((bytesRead = read(iFD, buffer, bufferSize)) > 0)
	{
		bytesWrite = write(oFD, buffer, bytesRead);
		printf("%d\n", bytesRead);
		if (bytesWrite != bytesRead)
		{
			fprintf(stderr, "Cannot write to file: %s\n", strerror(errno));
			return -1;
		}
	}

	if (bytesRead < 0)
	{
		fprintf(stderr, "Cannot read file: %s\n", strerror(errno));
		return -1;
	}

	return bytesRead;
}

int main (int argc, char *argv[])
{
	int opt = -1;
	int bufferSize = 4096;
	char* outfile = NULL;
	int numFiles = 0;
	int iFD = 0, oFD = 1;
	int ii;

	// Read Command Line Arguments
	while ((opt=getopt(argc, argv, "b:o:")) != -1)
	{
		switch (opt)
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

	// Initialize array of infiles
	char *infile[argc-optind];
	while(optind < argc)
	{
		infile[numFiles] = argv[optind];
		numFiles++;
		optind++;
	}

	if (outfile != NULL)
	{
		oFD = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (oFD == -1)
		{
			fprintf(stderr, "Cannot open output file: %s\n", strerror(errno));
			return -1;
		}
	}

	if (numFiles > 0)
	{
		for (ii = 0; ii < numFiles; ii++)
		{
			if(*infile[ii] != '-')
			{
				printf("Opening %s\n", infile[ii]);
				iFD = open(infile[ii], O_RDONLY);
				if (iFD == -1)
				{
					fprintf(stderr, "Cannot open input file: %s\n", strerror(errno));
					return -1;
				}
			}
			else
			{
				//set iFD to stdin
				iFD = 0;
			}

			if (processFiles(iFD, oFD, bufferSize) < 0)
			{
				printf("SIIII\n");
				return -1;
			}

			if(*infile[ii] != '-')
			{
				if (close(iFD) == -1)
				{
					fprintf(stderr, "Cannot close input file: %s\n", strerror(errno));
					return -1;
				}
			}
		}
	}
	else
	{
		if (processFiles(iFD, oFD, bufferSize) < -1){
			return -1;
		}
	}

	if (oFD != 2 && close(oFD) == -1)
	{
		fprintf(stderr, "Cannot close output file: %s\n", strerror(errno));
		return -1;
	}

	return 0;

}
