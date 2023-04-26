#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
    pid_t pid;
    if(pid = fork() == 0) // hijo
    {
        pid_t sid = setsid();
        int fd = open("/tmp/demonio.out", O_CREAT | O_RDONLY, 0666);
        dup2(fd, 1);
        int fderr = open("/tmp/demonio.err", O_CREAT | O_RDONLY, 0666);
        dup2(fderr, 2);
        int null = open("/dev/null", O_CREAT | O_RDONLY, 0666);
        dup2(null, 0);
        execvp(argv[1], &argv[1]);
    }
    else if(pid > 0) // padre
    {
    }
    else // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return 0;
}
