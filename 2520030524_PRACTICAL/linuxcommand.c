#include <stdio.h>
#include <unistd.h>
int main()
{
printf("Before exec()\n");
execlp("ls","ls","-l",NULL);
printf("This will not execute.\n");
return 0;
}


