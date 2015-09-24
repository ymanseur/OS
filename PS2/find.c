// Yacine Manseur
// Cooper Union Fall 2015
// ECE 357: Operating Systems
// Problem Set 2
// find.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

int find (int uid, int mtime, char* startingPath, char* currentDir)
{
	DIR *dirp;
	struct dirent *de;
	struct stat st;

	if (!(dirp=opendir(startingPath)))
	{
		fprintf(stderr, "Cannot open directory %s: %s\n", startingPath, strerror(errno));
		return -1;
	}

	while (de=readdir(dirp))
	{
		char nodePathName[255] = "\0";

		// Get the path name of the node
		if(strcmp(de->d_name, ".") == 0)
		{
			strncat(nodePathName, currentDir, sizeof(nodePathName) + sizeof(currentDir));
		}
		else
		{
			strncat(nodePathName, currentDir, sizeof(nodePathName) + sizeof(currentDir));
			strncat(nodePathName, "/", sizeof(nodePathName) + sizeof("/"));
			strncat(nodePathName, de->d_name, sizeof(nodePathName) + sizeof(de->d_name));
		}

		// Get stats for current node
		if (lstat(nodePathName, &st) == -1){
			fprintf(stderr, "Error getting link status for %s\n", nodePathName);
			continue;
		}

		// Omit entries that don't correspond to the given uid
		if (uid != st.st_uid)
			continue;

		// Get owner of the node
		struct passwd *user = getpwuid(st.st_uid);

		// Get group owner of the node
		struct group *gid = getgrgid(st.st_gid);

		// Get modification time

	}

	closedir(dirp);

	return 0;
}

int main (int argc, char *argv[]) 
{
	int opt = -1;
	int uid = 0;
	struct passwd *pwd;
	int mtime = 0;
	char* startingPath = ".";

	while ( (opt = getopt(argc, argv, "u:m:")) != -1)
	{
		switch (opt)
		{
			case 'u':
				printf("%s\n", optarg);
				// Check if input is the name or the uid number
				if (sscanf(optarg, "%d", &uid) == 1)
					uid = atoi(optarg);
				else
					if((pwd = getpwnam(optarg)) == NULL)
					{
						fprintf(stderr, "User %s cannot be found.\n", optarg);
						return -1;
					}
					uid = pwd->pw_uid;
				break;
			case 'm':
				if ( atoi(optarg) == 0)
				{
					fprintf(stderr, "Usage: incorrect argument. Must be an integer.\n");
					return -1;
				}
				else
					mtime = atoi(optarg);
				break;
			case '?':
				if (optopt == 'u' || optopt == 'm')
				{
					fprintf(stderr, "Usage: -%c missing argument.\n", optopt);
					return -1;
				}
				else
				{
					fprintf(stderr, "Usage: %s [-u user] [-m mtime]\n", argv[0]);
					return -1;
				}
				break;
			default:
				fprintf(stderr, "Usage: %s [-u user] [-m mtime]\n", argv[0]);
				return -1;
		}
	}

	if (optind < argc)
	{
		startingPath = argv[optind];
	}

	if (find(uid, mtime, startingPath, startingPath) == -1)
		return -1;

	return 0;

}