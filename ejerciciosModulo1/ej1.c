#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    setuid(0);
    
    if (errno == EINVAL)
    {
        printf("The value of uid is not valid.");
    }
    else if (errno == EPERM)
    {
        printf("The proccess does not have appropiate privilges.");
    }
    
    
    return 1;
}
