#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    struct utsname *buf = malloc(sizeof(struct utsname));

    uname(buf);

    printf("sysname: %s [operating system name] \n", buf->sysname);
    printf("nodename: %s [name within network] \n", buf->nodename);
    printf("relase: %s [operating system release] \n", buf->release);
    printf("versionsysname: %s [operating system version] \n", buf->version);
    printf("machine: %s [hardware identifier] \n", buf->machine);
    printf("domain_name: %s [NIS or YP domain name] \n", buf->machine);

    free(buf);

    return 0;
}
