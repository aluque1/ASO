#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int terminar = 0;
void shandler(int signo)
{
    char msj[] = "signal capturada\n";
    terminar = 1;
    if (signo == SIGUSR1)
        write(1, msj, strlen(msj));
    return;
}

int main(void)
{
    pid_t pid;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);
    printf("Soy el proceso pid = %d\n", getpid());
    signal(SIGUSR1, SIG_IGN);

    if ((pid = fork()) != 0)
    {
        printf("Enviando SIGUSR1 ...\n");
        kill(pid, SIGUSR1);
    }
    else
    {
        printf("Soy el proceso %d, mi padre es %d\n", getpid(), getppid());
        signal(SIGUSR1, shandler);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        while (!terminar)
            ;
    }
    return 0;
}