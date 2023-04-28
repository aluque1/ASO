/* shell.c -- Shell elemental. Solo admite jobs con una orden */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

// Variables globales
struct listaJobs listaJobs = {NULL, NULL};

void capturaCtrlC(int sig);
void capturaCtrlBackslash(int sig);
void capturaCtrlZ(int sig);
void terminarJob(struct listaJobs *job, int esBg);

// Programa principal
int main(int argc, char **argv)
{
    char orden[LONG_MAX_ORDEN + 1];
    char *otraOrden = NULL;
    struct job JobNuevo;
    int esBg = 0;

    if (argc > 1)
    {
        fprintf(stderr, "Uso: Shell");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, capturaCtrlC) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGQUIT, capturaCtrlBackslash) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTSTP, capturaCtrlZ) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    // Repetir
    while (1)
    {

        // Si no hay job_en_foreground
        // Leer ordenes
        if (listaJobs.fg == NULL)
        {
            compruebaJobs(&listaJobs);
            if (!otraOrden)
            {
                if (leeOrden(stdin, orden))
                    break;
                otraOrden = orden;
            }

            // Si la orden no es vacia, analizarla y ejecutarla
            if (!analizaOrden(&otraOrden, &JobNuevo, &esBg) && JobNuevo.numProgs)
            {
                ejecutaOrden(&JobNuevo, &listaJobs, esBg);
            }
        }
        else
        {
            int stat;
            waitpid(listaJobs.fg->progs[0].pid, &stat, WUNTRACED);
            if (WIFEXITED(stat))
            {
                printf("\nJob [%d] terminado\n", listaJobs.fg->jobId);
                terminarJob(&listaJobs, esBg);
            }
            else if (WIFSIGNALED(stat))
            {
                printf("\nJob [%d] terminado por señal\n", listaJobs.fg->jobId);
                terminarJob(&listaJobs, esBg);
            }
            else if (WIFSTOPPED(stat))
            {
                printf("\nJob [%d] parado\n", listaJobs.fg->jobId);
                JobNuevo.estado = 1;
                listaJobs.fg->estado = 1;
                tcsetpgrp(STDIN_FILENO, getpid());
                listaJobs.fg = NULL;
            }
        }
    }
    // Salir del programa (codigo error)
    exit(EXIT_FAILURE);
}

void capturaCtrlC(int sig)
{
    if (listaJobs.fg)
    {
        kill(listaJobs.fg->pgrp, SIGINT);
    }
}

void capturaCtrlBackslash(int sig)
{
    if (listaJobs.fg)
    {
        kill(listaJobs.fg->pgrp, SIGQUIT);
    }
}

void capturaCtrlZ(int sig)
{
    if (listaJobs.fg)
    {
        kill(listaJobs.fg->pgrp, SIGSTOP);
    }
}

void terminarJob(struct listaJobs *job, int esBg)
{
    listaJobs.fg->runningProgs = 0;
    listaJobs.fg->estado = 3;
    eliminaJob(&listaJobs, listaJobs.fg->progs[0].pid, esBg);
    listaJobs.fg = NULL;
    if (tcsetpgrp(STDIN_FILENO, getpid()))
    {
        perror("tcsetpgrp error");
    }
}
