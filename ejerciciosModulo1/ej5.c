#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("ticks per second    : %ld \n", sysconf(_SC_CLK_TCK));
    printf("max child proc      : %ld \n", sysconf(_SC_CHILD_MAX));
    printf("max files           : %ld \n", sysconf(_SC_OPEN_MAX));
    return 0;
}
