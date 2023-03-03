#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("max child proc      : %ld \n", sysconf(_SC_CHILD_MAX));
    printf("max page size       : %ld \n", sysconf(_SC_PAGESIZE));
    printf("max arg length      : %ld \n", sysconf(_SC_ARG_MAX));
    printf("max files open      : %ld \n", sysconf(_SC_OPEN_MAX));
    printf("max links to a file : %ld \n", sysconf(_SC_SYMLOOP_MAX));
    printf("max path length     : %ld \n", sysconf(_PC_PATH_MAX));
    printf("max file name       : %ld \n", sysconf(_PC_NAME_MAX));

    return 0;
}
