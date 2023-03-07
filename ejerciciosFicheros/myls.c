#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>

extern char *optarg;

int do_ls(char *path, struct stat *stat_buff);

int main(int argc, char *argv[])
{
	int opt;
	char *path;
	char *name;
	DIR *dir;
	struct stat *stat_buff = malloc(sizeof(struct stat));
	struct stat *temp = malloc(sizeof(struct stat));
	struct dirent *buff = malloc(sizeof(struct dirent));

	while ((opt = getopt(argc, argv, "hd:f:")) != -1)
	{
		switch (opt)
		{
		case 'd':
			path = optarg;
			lstat(path, stat_buff);
			if (S_ISDIR(stat_buff->st_mode) == 1 && (stat_buff->st_mode & (S_IRUSR | S_IXUSR)))
			{
				if ((dir = opendir(path)) != NULL)
				{
					while ((buff = readdir(dir)) != NULL)
					{
						name = buff->d_name;
						lstat(name, temp);
						do_ls(name, temp);
					}
				}
			}
			break;

		case 'f':
			path = optarg;
			lstat(path, stat_buff);
			if (S_ISREG(stat_buff->st_mode) == 1)
			{
				do_ls(path, stat_buff);
			}
			break;
		default:
			fprintf(stderr, "Usage: %s [-d ruta_directorio][-f ruta_fichero_regular]\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}

		return 0;
	}
}

int do_ls(char *path, struct stat *stat_buff)
{
	char buf[PATH_MAX];
	if (S_ISREG(stat_buff->st_mode) == 1)
	{
		printf("%s (%0.2f kB, %d link)", path, (float)(stat_buff->st_size >> 10), stat_buff->st_nlink);
		if (stat_buff->st_mode & S_IXUSR)
		{
			printf("*\n");
		}
		else
		{
			printf("\n");
		}
	}
	else if (S_ISLNK(stat_buff->st_mode) == 1)
	{
		printf("%s (%s)\n", path, realpath(path, buf));
	}
	else if (S_ISDIR(stat_buff->st_mode) == 1)
	{
		printf("[%s] (%d link)\n", path, stat_buff->st_nlink);
	}
}
