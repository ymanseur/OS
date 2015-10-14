// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 3
// shell.c

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>

char* readLines(FILE *fp)
{
	char* buffer = (char *)malloc(sizeof(char) * 128);
	if (buffer == NULL)
	{
		fprintf(stderr, "Error allocating memory.\n");
		exit(1);
	}

	int c = getc(fp);
	int i = 0;
	while(c != '\n' && c != EOF)
	{
		buffer[i] = c;
		i++;
		c=getc(fp);
	}

	if (c == EOF)
		exit(0);

	buffer[i] = '\0';
	realloc(buffer, i + 1);
	return buffer;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int ii;

	if (argc > 1)
	{
		// Open given file for reading
		fp = fopen(argv[1], "r");
		if(fp == NULL)
		{
			fprintf(stderr, "Error reading file: %s\n", strerror(errno));
			exit(1);
		}
	}
	else
	{
		// Set fp to stdin if nothing specified
		fp = stdin;
	}
	
	char *line;
	while ((line = readLines(fp)) != NULL && !feof(fp))
	{
		// Line is a comment. Ignore it.
		if(line[0] == '#')
			continue;

		char *word;
		char **args = NULL;
		int numSpaces = 0;

		word = strtok(line, " ");
		while(word != NULL)
		{
			numSpaces++;
			args = realloc(args, sizeof(char*) * numSpaces);
			if(args == NULL)
			{
				fprintf(stderr, "Error allocating memory.\n");
				exit(1);
			}
			args[numSpaces-1] = word;
			word = strtok(NULL, " ");
		}

		//Set end of args to null
		args = realloc(args, sizeof(char*) * (numSpaces+1));
		args[numSpaces] = NULL;

		pid_t pid;
		struct rusage ru;
		int status;
		int flag = 0;;

		switch (pid=fork())
		{
			case -1:
				perror("Fork failed.");
				exit(1);
				break;
			case 0:
				if(numSpaces > 1)
				{
					char* path = NULL;
					int oldfd, newfd;

					if(strstr(args[numSpaces-1], "<"))
					{
						path = strstr(args[numSpaces-1], "<");
						path++;
						oldfd = open(path, O_RDONLY);
						// redirect to stdin
						newfd = 0;
						flag = -1;
					}
					else
					{
						if(strstr(args[numSpaces-1], "2>>"))
						{
							path = strstr(args[numSpaces-1], "2>>");
							path++;
							path++;
							path++;
							oldfd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IREAD | S_IWRITE);
							// redirect to stderr
							newfd = 2;
							flag = -1;
						}
						else if(strstr(args[numSpaces-1], ">>"))
						{
							path = strstr(args[numSpaces-1], ">>");
							path++;
							path++;
							oldfd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IREAD | S_IWRITE);
							// redirect to stdout
							newfd = 1;
							flag = -1;
						}
						else if(strstr(args[numSpaces-1], "2>"))
						{
							path = strstr(args[numSpaces-1], "2>");
							path++;
							path++;
							oldfd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
							// redirect to stderr
							newfd = 2;
							flag = -1;
						}
						else if(strstr(args[numSpaces-1], ">"))
						{
							path = strstr(args[numSpaces-1], ">");
							path++;
							oldfd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
							// redirect to stdout
							newfd = 1;
							flag = -1;
						}
					}

					if (oldfd < 0 || newfd < 0)
					{
						fprintf(stderr, "Error opening file: %s\n", strerror(errno));
						exit(1);
					}
					else
					{
						dup2(oldfd, newfd);
						close(oldfd);
						if (flag == -1)
							args[numSpaces-1] = '\0';
					}
				}

				fprintf(stderr, "Executing command %s", args[0]);

				if(numSpaces != 1)
					fprintf(stderr, " with arguments ");

				for(ii = 1; args[ii] != NULL; ii++)
					fprintf(stderr, "\"%s\" ", args[ii]);

				fprintf(stderr, "\n");

				if(execvp(args[0], args) == -1)
				{
					fprintf(stderr, "Error executing file: %s\n", strerror(errno));
					exit(1);
				}
				break;
			default:
				if(wait3(&status,0,&ru) == -1)
				{
					perror("wait3");
				}
				else
				{
					fprintf(stderr, "Child process %d returned with return code %d,\nconsuming %ld.%.6d real seconds, %ld.%.6d user, %ld.%.6d system\n", 
								pid,
								status,
								ru.ru_utime.tv_sec + ru.ru_stime.tv_sec,
								ru.ru_utime.tv_usec + ru.ru_stime.tv_usec,
								ru.ru_utime.tv_sec,
								ru.ru_utime.tv_usec,
								ru.ru_stime.tv_sec,
								ru.ru_stime.tv_usec);
				}
				break;
			free(line);
			free(args);
		}
	}

	exit(0);
}