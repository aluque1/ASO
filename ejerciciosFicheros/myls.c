#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

extern char* optarg;

int get_size(char *fname, size_t *blocks);
int get_size_dir(char *dname, size_t *blocks);

int main(int argc, char const *argv[])
{
    int opt;
    char* path;
    while ((opt = getopt(argc, argv, "d:f:")))
    {
        switch (opt){
        case 'd':
            path = optarg;
        case 'f':
            path = optarg;
        default: 
            fprintf(stderr, "Usage: %s [-d ruta_directorio][-f ruta_fichero_regular]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    struct stat *stat_buff = malloc(sizeof(struct stat));

	lstat(argv[1], stat_buff); //stat buffer init
    
    return 0;
}

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks){
	struct stat *stat_buff = malloc(sizeof(struct stat));
	lstat(fname, stat_buff);
	
	if (S_ISDIR(stat_buff->st_mode) == 1){
		get_size_dir(fname, blocks);
	} 
	
	*blocks += stat_buff->st_blocks;

	return EXIT_SUCCESS;
}

/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks){

	DIR *dir;
	char* name;
	struct dirent *buff = malloc(sizeof(struct dirent));

	if ((dir = opendir(dname)) != NULL) {
		while ((buff = readdir(dir)) != NULL){
			name = buff->d_name;
			if (strcmp(name,".") & strcmp(name,".."))
				get_size(name, blocks);
		}	
		closedir(dir);
		return EXIT_SUCCESS;
	} else {
		perror("");
		return EXIT_FAILURE;
	}
}