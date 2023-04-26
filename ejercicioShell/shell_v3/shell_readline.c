
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

void mostrarPrompt();
void setupshell();
void capturaCtrlC(int sig);
void capturaCtrlBackslash(int sig);


int  main (int argc, char **argv)
{
    int estado;
    // ejecución interactiva o script?
    int entrada;
    entrada = stdin;
    if (argc > 2) {
        fprintf(stderr, "argumentos no esperados; Uso: asoshell [<script>]\n");
        exit(1);

    } else if (argc == 2) {
        entrada = fopen(argv[1], "r");
        if (entrada<0) {
            perror("fopen");
            exit(1);
        }
    }

    //configurar señales,estructuras de datos (jobs...)
    setupshell();

    // shellloop
    while (1){
        int pidhijo,estadohijo;
        char * linea,cmd;
        // mostrar prompt
        mostrarPrompt();

        // leer linea y analizar comando (historia)
        linea = readline(); //Libreria readline GNU (ubuntu: libreadline-dev)
        cmd = analisisComando(linea);

        //ejecutar builtin o orden externa (background/foreground)
        if (esbuiltin(cmd)){
            ejecutarbuiltin(cmd);
        } else {
            pidhijo = fork();
            if (pidhijo == 0){
                ejecutarorden(cmd); // execvp

            } else {
                if (esbackground(cmd)){
                    //registrar job
                } else {
                    //waitpid(pidhijo, estadohijo, optiones)
                }
            }
        }
    }
    return estado;
}

void mostrarPrompt() {
    char *prompt = "AS:";
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if (strcmp(cwd, "/") == 0)
    {
        printf("%s/# ", prompt);
    }
    else
    {
        char *home = getenv("HOME");
        // overcomplication to replace home with ~ in the prompt
        if (home != NULL)
        {
            char *pos = strstr(cwd, home); // localize home in cwd
            if (pos != NULL)
            {
                char *new_cwd = malloc(strlen(cwd) - strlen(home) + 1);
                new_cwd[0] = '~';
                strcat(new_cwd, pos + strlen(home));
                strcpy(cwd, new_cwd);
                free(new_cwd);
            }
        }
        printf("%s%s# ", prompt, cwd);
    }
}

void setupshell() {
    //configurar señales,estructuras de datos (jobs...)
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




}

// TODO mandar dicha señal a todos los procesos del grupo de procesos
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