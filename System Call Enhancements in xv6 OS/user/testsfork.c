#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    pid =sfork();
    if(pid<0)
    {
        printf("error");
    }
    if(pid==0)
    {
        printf("child");
        char *data=(char *)0x80000000;
        *data='C';
        printf("%c",*data);
    }
    else
    {
        printf("parent");
        char *data=(char *)0x80000000;
        printf("%c",*data);
    }
    return 0;
}