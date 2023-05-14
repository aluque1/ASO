#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

/*
    Todos los procesos se hagan en BG por defecto
    Y dos ordenes internas: asoexit y asowait
    asoexit: forzara al shell la terminacion de todos los procesos que se hubieran lanzado previamente (si los hubiese)
    y esperara a su terminacion antes de salir del shell.
    asowait: forzara a la shell a cederle la terminal de control al ultimo proceso que se hubiera lanzado y estuviera
    aun en ejecucion (si lo hubiese) y esperar a su terminacion o suspension antes de recuperar la terminal de control
    para solicitar la siguiente orden.
*/

#define MAX_ARGS 10
#define LONG_MAX_ARG 100
char *copiarargumento(char *buf);
int ejecutaordenbg(char *argv[]);

pid_t *pids;
pid_t ultimopid;

int main(int argc, char **argv)
{
    char *listaargumentos[MAX_ARGS + 1];
    int numargs, status;
    char buffer[LONG_MAX_ARG];

    if (argc > 1)
    {
        fprintf(stderr, "este shell no admite argumentos\n");
        exit(1);
    }

    numargs = 0;
    while (numargs < MAX_ARGS)
    {
        /* solicitar orden y argumentos de forma incremental. argv[0]
        es el comando o la orden interna a ejecutar, el resto los argumentos */
        printf("argv[%d]? ", numargs);
        if (fgets(buffer, LONG_MAX_ARG, stdin) && (buffer[0] != '\n'))
        {
            listaargumentos[numargs++] = copiarargumento(buffer);
        }
        else
        {
            if (numargs > 0)
            {
                listaargumentos[numargs] = NULL; /* marcar el final de lista */

                /* comprobar el tipo de orden */
                if (strcmp(listaargumentos[0], "asoexit") == 0)
                {
                    if (ultimopid > 0)
                    {
                        waitpid(ultimopid, &status, 0);
                    }
                    exit(0);
                }
                /* asowait: forzara a la shell a cederle la terminal de control al ultimo proceso que se hubiera lanzado y estuviera
    aun en ejecucion (si lo hubiese) y esperar a su terminacion o suspension antes de recuperar la terminal de control
    para solicitar la siguiente orden. */
                else if (strcmp(listaargumentos[0], "asowait") == 0)
                {
                    if (ultimopid > 0)
                    {
                        waitpid(ultimopid, &status, 0);
                    }
                    numargs = 0;
                }
                else
                {
                    ultimopid = ejecutaordenbg(listaargumentos);
                    printf("ejecutandose orden %s, con pid %d\n", listaargumentos[0], ultimopid);
                    numargs = 0;
                }
            }
        }
    }
    return 0;
}

char *copiarargumento(char *buf)
{
    char *ptrc;
    buf[strlen(buf) - 1] = '\0'; /* elimina caracter \n */
    ptrc = malloc(strlen(buf) + 1);
    if (ptrc == NULL)
    {
        fprintf(stderr, "no hay memoria disponible en el heap\n");
        exit(1);
    }
    strcpy(ptrc, buf); /* copia caracteres */
    return ptrc;
}

pid_t ejecutaordenbg(char *argv[])
{
    /* ejecuta orden externa en background y devuelve el pid del proceso correspondiente */
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        /* proceso hijo */
        setpgid(0, 0);
        execvp(argv[0], argv);
        /* si llega aqui es que ha habido error */
        fprintf(stderr, "error en execvp: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid > 0)
    {
        setpgid(pid, pid);
        return pid;
    }
    else
    {
        fprintf(stderr, "error en fork: %s\n", strerror(errno));
        exit(1);
    }
}
