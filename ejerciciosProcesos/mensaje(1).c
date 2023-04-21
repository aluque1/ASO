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
int seconds;
volatile bool end = false;

void mensaje();
void alarm_handler(int signum);
void termination_handler(int signum);
void terminate_handler(int signum);

int main(int argc, char *argv[])
{
    struct timeval init_time, end_time;
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
            msg_glob = (char *) malloc((sizeof(char) * (strlen(argv[2])+2))); /* un carácter para el \n y otro carácter para el fin de cadena*/
            strcat(msg_glob,argv[2]);
            strcat(msg_glob,"\n");
            
        }
        gettimeofday(&init_time, NULL);
        mensaje();
        gettimeofday(&end_time, NULL);
    }

    
    
    printf("\nseconds since start : %lds\n", end_time.tv_sec-init_time.tv_sec); /* esto es tiempo de CPU*/

    return 0;
}

void termination_handler(int signum)
{
    char *term_msg = "\nCannot terminate using interupt signal; signum = 2\n";
    write(1, term_msg, strlen(term_msg));
}

void alarm_handler(int signum)
{
    write(1, msg_glob, strlen(msg_glob));
}

void terminate_handler(int signum)
{
    end = true;
}

void mensaje()
{
    /* Manejo de señal SIGINT */
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    
    /* manejo de SIGTERM */
    struct sigaction term_action;
    sigemptyset(&term_action.sa_mask);
    term_action.sa_flags = 0;
    term_action.sa_handler = &terminate_handler;
    sigaction(SIGTERM, &term_action, NULL);

    /* manejo de SIGALARM */
    // sigaction con sigalarm
    struct sigaction alarm_action;
    sigemptyset(&alarm_action.sa_mask);
    alarm_action.sa_flags = 0;
    alarm_action.sa_handler = &alarm_handler;
    sigaction(SIGALRM, &alarm_action, NULL);


    while (!end){
        printf("----\n");
        alarm(seconds);
        pause();
    }
    

}