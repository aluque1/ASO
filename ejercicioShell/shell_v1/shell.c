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

// Programa principal
int main(int argc, char **argv)
{

    char orden[LONG_MAX_ORDEN + 1];
    char *otraOrden = NULL;
    struct job JobNuevo;
    int esBg = 0;

    // Procesamiento argumentos de entrada (versión simplificada)
    if (argc > 1)
    {
        fprintf(stderr, "Uso: Shell");
        exit(EXIT_FAILURE);
    }

    // Ignorar la señal SIGTTOU, capturar SIGINT, SIGQUIT...
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
        if (!listaJobs.fg)
        {
            // Comprobar finalización de jobs
            compruebaJobs(&listaJobs);
            // Leer ordenes
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
            // Si hay job en foreground
            // Esperar a que acabe el proceso que se encuentra en foreground
            waitpid(listaJobs.fg->progs[0].pid, NULL, 0);
            // Recuperar el terminal de control
            tcsetpgrp(STDIN_FILENO, getpgid(0));
            // Si parada_desde_terminal
            if (WIFSTOPPED(listaJobs.fg->runningProgs))
            {
                // Informar de la parada
                fprintf(stdout, "[%d] Stopped\t%d\t%s\n", listaJobs.fg->jobId, listaJobs.fg->progs[0].pid, listaJobs.fg->texto);
                // Actualizar el estado del job y la lista
                listaJobs.fg->runningProgs = WSTOPSIG(listaJobs.fg->runningProgs);
            }
            else
            {
                // Eliminar el job de la lista
                eliminaJob(&listaJobs, listaJobs.fg->progs[0].pid, 0);
            }
        }

        // Salir del programa (codigo error)
        exit(EXIT_FAILURE);
    }
}


void capturaCtrlC(int sig)
{
    // Si hay job en foreground
    if (listaJobs.fg)
    {
        // Enviar señal SIGINT al proceso que se encuentra en foreground
        kill(listaJobs.fg->progs[0].pid, SIGINT);
    }
}

void capturaCtrlBackslash(int sig)
{
    // Si hay job en foreground
    if (listaJobs.fg)
    {
        // Enviar señal SIGQUIT al proceso que se encuentra en foreground
        kill(listaJobs.fg->progs[0].pid, SIGQUIT);
    }
}

void capturaCtrlZ(int sig)
{
    // Si hay job en foreground
    if (listaJobs.fg)
    {
        // Enviar señal SIGTSTP al proceso que se encuentra en foreground
        kill(listaJobs.fg->progs[0].pid, SIGTSTP);
    }
}