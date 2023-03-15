#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <sys/file.h>
#include <fcntl.h>


void do_cerrojo();

int main(int argc, char *argv[])
{
    struct stat *stat_buff = malloc(sizeof(struct stat));

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [ruta_fichero] -h (help) :: Use a file as the argument \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        do_cerrojo(argv[1]);
    }
    free(stat_buff);

    return 0;
}

void start_lock(short int type, struct flock *l){
    l->l_len = 0;
    l->l_start = 0;
    l->l_whence = SEEK_SET;
    l->l_type = type;
}

char *get_time(){
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *tiempo = malloc(20);
    strftime(tiempo, 20, "%F %T \n", tm);
    return tiempo;
}

void do_cerrojo(char *path)
{
    int fd;

    struct flock *l = malloc(sizeof(struct flock));

    fd = open(path, O_RDWR | O_APPEND | O_CREAT, 0666);

    start_lock(F_WRLCK, l);
    fcntl(fd, F_GETLK, l);

    if (l->l_type == F_UNLCK) // unlocked
    {
        start_lock(F_WRLCK, l);
        fcntl(fd, F_SETLK, l);

        /* esto estaba como
            write(fd, get_time(), sizeof(time));

            recordar que sizeof de un puntero te devuelve el 
            tamaño del puntero, no el tam del dato al que apunta
        */  

        write(fd, get_time(), strlen(time));
        sleep(5);

        start_lock(F_UNLCK, l);
        fcntl(fd, F_SETLK, l);        
    }
    else // locked
    {
        printf("File locked\n");
        exit(EXIT_SUCCESS);
    }
    free(l);
    close(fd);
}