#define _GNU_SOURCE // added so the vscode linter doesn't bug out

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
char *msg_glob = "mensaje predeterminado\n";
volatile int seconds;
volatile bool end = false;

void mensaje();
void alarm_handler(int signum);
void termination_handler(int signum);
void terminate_handler(int signum);
char* get_time();

int main(int argc, char *argv[])
{
    if (argc > 3 || argc < 2)
    {
        fprintf(stderr, "Usage: %s segundos ['mensaje_opcional']\n para terminar ejecucion kill -15 <pid>", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        seconds = atoi(argv[1]);
        if (argc == 3)
        {
            strcat(argv[2], "\n");
            msg_glob = argv[2];
        }
        mensaje();
    }

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("\nseconds since start : %lds\n", usage.ru_utime.tv_sec);

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

void terminate_handler(int signum)
{
    end = true;
}

void mensaje()
{
    /* Manejo de señal SIGINT */
    struct sigaction act, oact_interupt;

    act.sa_handler = &termination_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, NULL, &oact_interupt);
    if (oact_interupt.sa_handler != SIG_IGN)
    {
        sigaction(SIGINT, &act, NULL);
    }

    /* manejo de SIGALARM */
    // sigaction con sigalarm
    struct sigaction alarm_action, oact_alarm;
    alarm_action.sa_handler = &alarm_handler;
    sigaction(SIGALRM, &alarm_action, NULL);

    sigaction(SIGALRM, NULL, &oact_alarm);
    if (oact_alarm.sa_handler != SIG_IGN)
    {
        sigaction(SIGALRM, &alarm_action, NULL);
    }

    alarm(seconds);

    /* manejo de SIGTERM */
    struct sigaction term_action;
    term_action.sa_handler = &terminate_handler;
    sigaction(SIGTERM, &term_action, NULL);

    while (!end)
        ; // seguir
}
