#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>

extern char *optarg;

int main(int argc, char *argv[])
{
	char buf[PATH_MAX];
	int opt;
	char *path;
	struct stat *stat_buff = malloc(sizeof(struct stat));

	while ((opt = getopt(argc, argv, "d:f:")) != -1)
	{
		{
			switch (opt)
			{
			case 'd':
				path = optarg;
				lstat(path, stat_buff);
				if (S_ISREG(stat_buff->st_mode) == 1)
				{
					printf("%s (%f kB, %d link)", basename(path), stat_buff->st_size / 1024, stat_buff->st_nlink);
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
					printf("%s (%s)\n", basename(path), realpath(path, buf));
				}
				else if (S_ISDIR(stat_buff->st_mode) == 1)
				{
					printf("[%s] (%d link)\n", basename(path), stat_buff->st_nlink);
				}

			case 'f':
				if (S_ISREG(stat_buff->st_mode) == 1)
				{
					printf("%s (inodo %d, %f kB )", basename(path), stat_buff->st_ino, stat_buff->st_size / 1024);
					if (stat_buff->st_mode & S_IXUSR)
					{
						printf("*\n");
					}
					else
					{
						printf("\n");
					}
				}
			default:
				fprintf(stderr, "Usage: %s [-d ruta_directorio][-f ruta_fichero_regular]\n", argv[0]);
				exit(EXIT_FAILURE);
			}
		}
		return 0;
	}
}