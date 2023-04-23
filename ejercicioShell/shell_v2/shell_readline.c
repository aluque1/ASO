/* #include <stdio.h>
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
        linea= readline(NULL); //Libreria readline GNU (ubuntu: libreadline-dev)
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
    char * prompt = "asoshell> ";
    printf("%s", prompt);
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
}

void capturaCtrlBackslash(int sig)
{
}
 */