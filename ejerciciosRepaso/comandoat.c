#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


// at secs (comando)

int main(int argc, char const *argv[])
{
    pid_t pid;
    int secs;
    char *comando;
    secs = atoi(argv[1]);
    comando = argv[2];

    if (pid = fork() == 0) // hijo
    {
        signal(SIGALRM, &manejador);
        alarm(secs);
        pause();
    }
    else if (pid > 0) // padre
    {
        /* code */
    }
    else // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    execlp(comando, &comando, NULL);

    return 0;
}
