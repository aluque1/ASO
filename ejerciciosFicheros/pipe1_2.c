#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *fifo = "mituberia";

    mkfifo(fifo, 0777);


    int fd = open(fifo, O_WRONLY | O_APPEND);
    
    char *msg = argv[1];
    write(fd, msg, strlen(msg));

    close(fd);
    return 0;
}