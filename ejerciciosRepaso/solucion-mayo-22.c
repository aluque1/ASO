#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    ##################################################
    #                                                #
    #                 Ejercicio 1:                   #
    #                                                #
    ##################################################
 */

// 1.a
/*
    se podria hacer la comprobacion de que lo que se pasa por argv[1] es verdaderamente un directorio usando lstat y comprobando
    el valor del st_mode con S_ISDIR(st_mode)
*/

// 1.b
/*
    estado->st_nlink es el numero de enlaces duros que tiene el fichero, si es 1 es que no hay ningun otro fichero que apunte a el
*/

// 1.c
/*
    si el directorio no tiene permisos de lectura para el usuario que ejecuta el programa, no se podra leer el contenido del
    directorio y devolvera -1 y errno se pondra a EACCES
*/

/*
    ##################################################
    #                                                #
    #                 Ejercicio 2:                   #
    #                                                #
    ##################################################
 */

// 2.a
/*
    Si, se puede usar una tuberia sin nombre (pipe) para comunicar estos dos hijos. Esto se debe a que un pipe sin
    nombre se usa para comunicar 2 procesos relacionados entre si, y en este caso los dos hijos son hijos del mismo padre
*/
int main(int argc, char *argv[])
{
    int fds[2];
    pid_t pid1, pid2;
    if (pipe(fds) == -1)
    {
        perror("pipe");
        return EXIT_FAILURE;
    }
    if (pid1 = fork()) // padre 1
    {
        if (pid2 = fork()) // padre 2
        {
            close(fds[0]); // cerramos un extremo
            close(fds[1]); // cerramos el otro extremo

            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
        else // hijo 2
        {
            dup2(fds[0], STDIN_FILENO); // rerouting of stdin
            close(fds[0]);              // cerramos un extremo
            close(fds[1]);              // cerramos el otro extremo creado por dupe
        }
    }
    else // hijo 1
    {
        dup2(fds[1], STDOUT_FILENO); // rerouting of stdout
        close(fds[0]);               // cerramos un extremo
        close(fds[1]);               // cerramos el otro extremo creado por dupe
    }
}

// 2.b
/*
    Una de las ventajas que muestran los piper con nombre es que se pueden usar para comunicar procesos que no estan relacionados. 
    En este caso esta ventaja no se podria aprovechar ya que los dos hijos son hijos del mismo padre. Otra ventaja es que se pueden
    usar para comunicar procesos que no se ejecutan en el mismo orden, pero en este caso los dos hijos se ejecutan en el mismo orden
    que el padre. Con los piper con nombre se puede controlar los permisos de lectura y escritura de los procesos que se comunican
    pero aqui no nos importa mucho.
*/

/*
    ##################################################
    #                                                #
    #                 Ejercicio 3:                   #
    #                                                #
    ##################################################
 */

// 3.a
/*
    No, no se puede usar waitpid para que un hijo espere al padre ya que un waitpid solo se puedo usar para esperar a un hijo   
*/

// 3.b
/*
    Bajo la circunstancia de que el hijo se ejecute antes que el padre ya que el hijo es el que inicia el handler para que
    terminar se ponga a 1, saliendo de ese bucle while
*/

// 3.c
