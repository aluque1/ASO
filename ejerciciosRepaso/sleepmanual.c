//#define _GNU_SOURCE // added so the vscode linter doesn't bug out

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>

jmp_buf env;

void manejador(int sig)
{
    longjmp(env, 1);
}

void dormir(int secs)
{
    void (*sig_old)();
    int alarm_ant;

    if(secs <= 0) return;

    sig_old = signal(SIGALRM, manejador);/* <---+
     al no ser atomicos puede que se produzca la señal. Para immplementar esto tenemos que usar un setjmp() y un longjmp()
    que son GOTO */
    
    if (setjmp(env) == 0) // seteamos el "GOTO" aqui
    {
        alarm_ant = alarm(secs);
        pause();
    }
    else // ha sonado la alarma entonces : restauramos el alarm antiguo
    {
        signal(SIGALRM, sig_old);
        alarm(alarm_ant);
    }
}