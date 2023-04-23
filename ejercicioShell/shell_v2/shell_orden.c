/* shell_orden.c -- rutinas relativas a tratamiento de ordenes */
#define _GNU_SOURCE // added so the vscode linter doesn't bug out

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <linux/limits.h>
#include "shell.h"

extern char **environ;

// Lee una línea de ordenes del fichero fuente
int leeOrden(FILE *fuente, char *orden)
{
    if (fuente == stdin)
    {
        printf("# ");
        fflush(stdout);
    }

    if (!fgets(orden, LONG_MAX_ORDEN, fuente))
    {
        if (fuente == stdin)
            printf("\n");
        return 1;
    }

    // suprime el salto de linea final
    orden[strlen(orden) - 1] = '\0';

    return 0;
}

// Analiza la orden y rellena los comandos de la estructura job
int analizaOrden(char **ordenPtr, struct job *job, int *esBg)
{

    /* Devuelve orden->numProgs a 0 si no hay ninguna orden (ej. linea vacia).

       Si encuentra una orden valida, ordenPtr apunta al comienzo de la orden
       siguiente (si la orden original tuviera mas de un trabajo asociado)
       o NULL (si no hay mas ordenes presentes).
    */

    char *orden;
    char *retornoOrden = NULL;
    char *orig, *buf;
    int argc = 0;
    int acabado = 0;
    int argvAsignado;
    char comilla = '\0';
    int cuenta;
    struct ProgHijo *prog;

    // Salta los espacios iniciales
    while (**ordenPtr && isspace(**ordenPtr))
        (*ordenPtr)++;

    // Trata las lineas vacias
    if (!**ordenPtr)
    {
        job->numProgs = 0;
        *ordenPtr = NULL;
        return 0;
    }

    // Comienza el analisis de la orden
    *esBg = 0;
    job->numProgs = 1;
    job->progs = malloc(sizeof(*job->progs));

    // Fragmenta la linea de orden en argumentos
    job->ordenBuf = orden = calloc(1, strlen(*ordenPtr) + MAX_ARGS);
    job->texto = NULL;
    prog = job->progs;

    /* Hacemos que los elementos de argv apunten al interior de la cadena.
       Al obtener nueva memoria nos libramos de tener que acabar
       en NULL las cadenas y hace que el resto del codigo parezca
       un poco mas limpio (aunque menos eficente)
    */
    argvAsignado = 5; // Simplificación
    prog->argv = malloc(sizeof(*prog->argv) * argvAsignado);
    prog->argv[0] = job->ordenBuf;
    buf = orden;
    orig = *ordenPtr;

    // Procesamiento caracter a caracter
    while (*orig && !acabado)
    {
        if (comilla == *orig)
        {
            comilla = '\0';
        }
        else if (comilla)
        {
            if (*orig == '\\')
            {
                orig++;
                if (!*orig)
                {
                    fprintf(stderr, "Falta un caracter detras de \\\n");
                    liberaJob(job);
                    return 1;
                }

                // En shell, "\'" deberia generar \'
                if (*orig != comilla)
                    *buf++ = '\\';
            }
            *buf++ = *orig;
        }
        else if (isspace(*orig))
        {
            if (*prog->argv[argc])
            {
                buf++, argc++;
                // +1 aqui deja sitio para el NULL que acaba argv
                if ((argc + 1) == argvAsignado)
                {
                    argvAsignado += 5;
                    prog->argv = realloc(prog->argv,
                                         sizeof(*prog->argv) * argvAsignado);
                }
                prog->argv[argc] = buf;
            }
        }
        else
            switch (*orig)
            {
            case '"':
            case '\'':
                comilla = *orig;
                break;

            case '#': // comentario
                acabado = 1;
                break;

            case '&': // background
                *esBg = 1;
            case ';': // multiples ordenes
                acabado = 1;
                retornoOrden = *ordenPtr + (orig - *ordenPtr) + 1;
                break;

            case '\\':
                orig++;
                if (!*orig)
                {
                    liberaJob(job);
                    fprintf(stderr, "Falta un caracter detras de \\\n");
                    return 1;
                }
                // continua
            default:
                *buf++ = *orig;
            }

        orig++;
    }

    if (*prog->argv[argc])
    {
        argc++;
    }

    // Chequeo de seguridad
    if (!argc)
    {
        // Si no existen argumentos (orden vacia) liberar la memoria y
        // preparar ordenPtr para continuar el procesamiento de la linea
        liberaJob(job);
        *ordenPtr = retornoOrden;
        return 1;
    }

    // Terminar argv por un puntero nulo
    prog->argv[argc] = NULL;

    // Copiar el fragmento de linea al campo texto
    if (!retornoOrden)
    {
        job->texto = malloc(strlen(*ordenPtr) + 1);
        strcpy(job->texto, *ordenPtr);
    }
    else
    {
        // Se dejan espacios al final, lo cual es un poco descuidado
        cuenta = retornoOrden - *ordenPtr;
        job->texto = malloc(cuenta + 1);
        strncpy(job->texto, *ordenPtr, cuenta);
        job->texto[cuenta] = '\0';
    }

    // Preparar la linea para el procesamiento del resto de ordenes
    *ordenPtr = retornoOrden;

    return 0;
}

// Implementación ordenes internas con chequeo de errores elemental:

void ord_exit(struct job *job, struct listaJobs *listaJobs, int esBg)
{

    // Finalizar todos los jobs

    // Salir del programa
    exit(EXIT_SUCCESS);
}

void ord_pwd(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("getcwd() error");
}

void ord_cd(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    char *dir = job->progs->argv[1];
    if (dir == NULL)
        dir = getenv("HOME");

    if (chdir(dir) != 0)
        perror("chdir() error");
}

void ord_jobs(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    struct job *job_act, *job_prev;
    for (job_act = listaJobs->primero, job_prev = NULL; job_act; job_prev = job_act, job_act = job_act->sigue)
    {
        if (job_prev)
        {
            printf("[%d] ", job_prev->jobId);
            // TODO ver si esto es a lo que se refiere el enunciado
            printf("%s \n", job_prev->texto);
        }
    }
}

// TODO check if this works as intended
void ord_wait(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    int id = atoi(job->progs->argv[1]);
    struct job *job_encontrado, *job_prev;
    job_encontrado = buscaJob(listaJobs, id);
    job_prev = listaJobs->primero;

    listaJobs->fg = job_encontrado;
    waitpid(job_encontrado->pgrp, NULL, 0);
    listaJobs->fg = job_prev;
}

void ord_kill(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    int id = atoi(job->progs->argv[1]);
    struct job *job_encontrado;
    job_encontrado = buscaJob(listaJobs, id);

    if (job_encontrado)
    {
        kill(job_encontrado->pgrp, SIGKILL);
    }
}

void ord_stop(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    int id = atoi(job->progs->argv[1]);
    struct job *job_encontrado;
    job_encontrado = buscaJob(listaJobs, id);

    if (job_encontrado)
    {
        kill(job_encontrado->pgrp, SIGSTOP);
    }
}

void ord_fg(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    int id = atoi(job->progs->argv[1]);
    struct job *job_encontrado, *job_prev;
    job_encontrado = buscaJob(listaJobs, id);

    if (job_encontrado)
    {
        job_prev = listaJobs->fg;
        listaJobs->fg = job_encontrado;
        kill(job_encontrado->pgrp, SIGCONT);
        waitpid(job_encontrado->pgrp, NULL, 0);
        listaJobs->fg = job_prev;
    }
}

void ord_bg(struct job *job, struct listaJobs *listaJobs, int esBg)
{

    // Pasar el job N a background y mandar SIGCONT
    // Si existe y esta parado mandar señal SIGCONT y actualizar su estado
    int id = atoi(job->progs->argv[1]);
    struct job *job_encontrado;
    job_encontrado = buscaJob(listaJobs, id);

    // Queda ver como se mandan los distinos jobs a background
}

// Convierte un struct timeval en segundos (s) y milisegundos (ms)
void timeval_to_secs(struct timeval *tvp, time_t *s, int *ms)
{
    int rest;

    *s = tvp->tv_sec;

    *ms = tvp->tv_usec % 1000000;
    rest = *ms % 1000;
    *ms = (*ms * 1000) / 1000000;
    if (rest >= 500)
        *ms += 1;

    // Comprobacion adicional
    if (*ms >= 1000)
    {
        *s += 1;
        *ms -= 1000;
    }
}

void ord_times(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *tiempo = malloc(9);
    strftime(tiempo, 9, "%T", &tm);
    printf("Current time: %s\n", tiempo);
}

void ord_date(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char s[64];
    strftime(s, sizeof(s), "%F", &tm);
    printf("Current date: %s\n", s);
}

// Ejecución de un comando externo
void ord_externa(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    // Duplicar proceso

    // Hijo

    // Crear un nuevo grupo de procesos

    // Ejecutar programa con los argumentos adecuados

    // Si la llamada a execvp retorna es que ha habido un error

    // Padre

    // Crear un nuevo trabajo a partir de la informacion de job

    // Insertar Job en la lista (el jobID se asigna de manera automatica

    // Si no se ejecuta en background

    // Cederle el terminal de control y actualizar listaJobs->fg

    // De lo contrario, informar por pantalla de su ejecucion
    pid_t pid = fork();

    if (pid != 0)
    {
        // Si es foreground
        if (esBg)
        {
            tcsetpgrp(STDIN_FILENO, pid);
            listaJobs->fg = job;
        }
        // Si es background
        else
        {
            printf("[%d] %d\n", job->jobId, pid);
        }
    }
    else if (pid == 0)
    {
        // crea un nuevo grupo de procesos
        setpgid(0, 0);
        // ejecuta el programa con los argumentos adecuados
        execvp(job->progs[0].argv[0], job->progs[0].argv);
    }
    else
    {
        perror("Fork not created");
    }
}

// Realiza la ejecución de la orden
void ejecutaOrden(struct job *job, struct listaJobs *listaJobs, int esBg)
{
    char *orden = job->progs[0].argv[0];

    // Si es orden interna ejecutar la acción apropiada
    if (!strcmp("exit", orden))
        ord_exit(job, listaJobs, esBg);
    else if (!strcmp("pwd", orden))
        ord_pwd(job, listaJobs, esBg);
    else if (!strcmp("cd", orden))
        ord_cd(job, listaJobs, esBg);
    else if (!strcmp("jobs", orden))
        ord_jobs(job, listaJobs, esBg);
    else if (!strcmp("wait", orden))
        ord_wait(job, listaJobs, esBg);
    else if (!strcmp("kill", orden))
        ord_kill(job, listaJobs, esBg);
    else if (!strcmp("stop", orden))
        ord_stop(job, listaJobs, esBg);
    else if (!strcmp("fg", orden))
        ord_fg(job, listaJobs, esBg);
    else if (!strcmp("bg", orden))
        ord_bg(job, listaJobs, esBg);
    else if (!strcmp("time", orden))
        ord_times(job, listaJobs, esBg);
    else if (!strcmp("date", orden))
        ord_date(job, listaJobs, esBg);
    // Si no, ejecutar el comando externo
    else
        ord_externa(job, listaJobs, esBg);
}
