#define _GNU_SOURCE // added so the vscode linter doesn't bug out

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>

char *msg_glob = "mensaje predeterminado\n";
volatile int seconds;

void mensaje(char *msg);

int main(int argc, char *argv[])
{
    if (argc > 3 || argc < 2)
    {
        fprintf(stderr, "Usage: %s segundos [mensaje_opcional]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        seconds = atoi(argv[1]);
        mensaje(argv[2]);
    }
    return 0;
}

void termination_handler(int signum)
{
    static char *term_msg = "\nCannot terminate using interupt signal; signum = 2\n";
    write(1, term_msg, strlen(term_msg));
}

void alarm_handler(int signum)
{
    static char *working_message;
    working_message = msg_glob;
    write(1, working_message, strlen(working_message));
    alarm(seconds);
}

void mensaje(char *msg)
{
    /* Manejo de señal SIGINT */
    struct sigaction new_action, old_action;

    new_action.sa_handler = &termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigaction(SIGINT, &new_action, NULL);
    }

    /* manejo de SIGALARM */
    // sigaction con sigalarm
    struct sigaction alarm_action;
    alarm_action.sa_handler = &alarm_handler;
    sigaction(SIGALRM, &alarm_action, NULL);

    alarm(seconds);

    /* int getrusage(int who, struct rusage *usage);
        who : RUSAGE_CHILDREN, RUSAGE_SELF
        struct rusage  {struct timeval ru_utime; // user time
                        struct timeval ru_stime; // sys CPU time
                        ...}
    */
    while (1)
    {
    }
    // when terminate I have to do getrusage
}
