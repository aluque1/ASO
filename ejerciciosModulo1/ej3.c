#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    for (int i = 0; i < 134; i++)
    {
        printf("error number %d : %s \n", i, strerror(i));
    }
    return 0;
}
