/* shell.c -- Shell elemental. Solo admite jobs con una orden */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

// Variables globales
struct listaJobs listaJobs = {NULL, NULL};

void capturaCtrlC(int sig);
void capturaCtrlBackslash(int sig);

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

    // Repetir
    while (1)
    {

        // Si no hay job_en_foreground
        /*  if (!listaJobs.fg)
         {
             // comprueba jobs
             compruebaJobs(&listaJobs);
         */
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
        /* } */
        /*   else
          {
              int *status;
              // Esperar a que acabe el proceso que se encuentra en foreground
              waitpid(listaJobs.fg->progs[0].pid, status, WUNTRACED);
              // Recuperar el terminal de control
              tcsetpgrp(STDIN_FILENO, getpgrp());
              // Si parada_desde_terminal
              if (WIFSTOPPED(*status))
              {
                  printf("Job %d parado\n", listaJobs.fg->jobId);
                  // Actualizar el estado del job y la listaç
              }
              // (Else) si no

              // Eliminar el job de la lista
          } */
    }

    // Salir del programa (codigo error)
    exit(EXIT_FAILURE);
}

// TODO mandar dicha señal a todos los procesos del grupo de procesos
void capturaCtrlC(int sig)
{
    if (listaJobs.fg)
    {
    }
}

void capturaCtrlBackslash(int sig)
{
    if (listaJobs.fg)
    {
    }
}
