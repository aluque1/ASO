#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    setuid(0);
    
    if (errno == EINVAL)
    {
        printf("The value of uid is not valid.\n");
    }
    else if (errno == EPERM)
    {
        printf("The proccess does not have appropiate privilges.\n");
    }

    printf("The numeric value is: %d \n", errno);    
    printf("The string associated is: %s \n", strerror(errno));
    return 1;
}
