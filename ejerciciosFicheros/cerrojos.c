#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <sys/time.h>
#include <fcntl.h>

extern char *optarg;

void do_cerrojo();

int main(int argc, char *argv[])
{
    int opt;
    char *path;
    struct stat *stat_buff = malloc(sizeof(struct stat));

    while ((opt = getopt(argc, argv, "h:")) != -1)
    {
        switch (opt)
        {

        case 'h':
            fprintf(stderr, "Usage: %s [ruta_fichero] -h (help)\n", argv[0]);
            exit(EXIT_FAILURE);
            break;

        default:
            path = optarg;
            lstat(path, stat_buff);

            if (S_ISREG(stat_buff->st_mode) == 1)
            {
                do_cerrojo(path);
            }
            else
            {
                fprintf(stderr, "Usage: %s [ruta_fichero] -h (help) :: Use a file as the argument \n", argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    free(stat_buff);

    return 0;
}

void time_of_day(__time_t *hours, __time_t *mins, __time_t *secs)
{
    struct timeval *tv = malloc(sizeof(struct timeval));

    gettimeofday(tv, NULL);
    *hours = tv->tv_sec/3600;
    *mins = ((tv->tv_sec - (*hours*3600)) / 60);
    *secs = tv->tv_sec - (*hours*3600) - (*mins*60);

    free(tv);
}

void do_cerrojo(char *path)
{
    int fd;
    __time_t hours, mins, secs;
    char *time;
    char *fname;
    fname = basename(path); // parsing of the path to get the filename

    struct flock *l = malloc(sizeof(struct flock));
    struct flock *locking = malloc(sizeof(struct flock));
    locking->l_type = F_WRLCK; // r/w lock
    locking->l_whence = SEEK_SET;
    locking->l_start = 0;
    locking->l_len = 0;
    locking->l_pid = getpid();
    fd = open(fname, O_RDWR | O_EXCL | O_CREAT);
    fcntl(fd, F_GETLK, l);

    if (l->l_type != F_UNLCK) // locked
    {
        exit(EXIT_SUCCESS);
    }
    else // unlocked
    {
        fcntl(fd, F_SETLKW, locking); // set lock

        time_of_day(&hours, &mins, &secs);
        sprintf(time, "%ld:%ld:%ld", hours, mins, secs); // getting the time and formating for the write
        write(fd, time, sizeof(time));

        sleep(40);                    // sleep(40);
        locking->l_type = F_UNLCK;    // set the type to unlock
        fcntl(fd, F_SETLKW, locking); // unset lock
    }
    free(l);
    close(fd);
}