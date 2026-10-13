#include <stdio.h>
#include <unistd.h>
int main()
{
for (int i=1;i<=3;i++)
{
if(fork()==0)
{
printf("Child %d PID = %d\n", i, getpid());
return 0;
}
}
return 0;
}
