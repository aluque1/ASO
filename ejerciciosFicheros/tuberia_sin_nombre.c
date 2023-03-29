#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return EXIT_FAILURE;
    }

    int pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if (pid1 == 0)
    {
        
        // Hijo 1 (comando de argv[2])
        // rerouting of stdout

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]); // cerramos un extremo
        close(pipefd[1]); // cerramos el otro extremo creado por dupe

         execlp(argv[1], argv[2], NULL);
        //execlp("ls", "", NULL);
    }

    // como usamos exec no hace falta if para el padre
    
    int pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if (pid2 == 0)
    {
        // Hijo 2 (comando de argv[4])
        // rerouting of stdin
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]); // cerramos un extremo
        close(pipefd[1]); // cerramos el otro extremo creado por dupe
        // execlp 
         execlp(argv[3], argv[4], NULL);
        //execlp("wc", "", NULL);
    }

    close(pipefd[0]); // cerramos un extremo
    close(pipefd[1]); // cerramos el otro extremo
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
