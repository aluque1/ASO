#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TIMEOUT 30
#define BUF_LEN 1024

int main(void)
{
    
    struct timeval tv;
    fd_set readfds;
    int ret;
    char buf[BUF_LEN + 1];
    int len;

    FD_ZERO(&readfds);

    /* Wait up to five seconds. */
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    /*
    ret: numero de fds preparados para I/O (puede ser 0 si timeout), -1 si error, ejemplo: EINTR (interrupcion)
    */
    if (ret == -1)
    {
        perror("select");
        return 1;
    }
    else if (!ret)
    {
        printf("Han pasado %d segundos.\n", TIMEOUT);
        return 0;
    }

    /*
        read from the two pipes named tuberia2 and mituberia with select to see if something has
        been written in them
    */

    int tuberia2 = open("tuberia2", O_NONBLOCK);
    int mituberia = open("mituberia", O_NONBLOCK);
    if (FD_ISSET(tuberia2, &readfds))
    {
        len = read(tuberia2, buf, BUF_LEN);
        if (len == -1)
        {
            perror("read");
            return 1;
        }
        if (len)
        {
            buf[len] = '\0';
            printf("Hemos leido de la tuberia2: %s\n", buf);
        }
        return 0;
    }
    if (FD_ISSET(mituberia, &readfds))
    {
        len = read(mituberia, buf, BUF_LEN);
        if (len == -1)
        {
            perror("read");
            return 1;
        }
        if (len)
        {
            buf[len] = '\0';
            printf("Hemos leido de la mituberia: %s\n", buf);
        }
        return 0;
    }
    fprintf(stderr, "se ha producido un error!\n");
    return 1;
}