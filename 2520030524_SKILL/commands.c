#include<stdio.h>
#include<string.h>
#include<unistd.h>
int main()
{
char command[100];
while(1)
{
printf("2520030524_Advaith$ ");
fgets(command, sizeof(command), stdin);
command[strcspn(command, "\n")]= '\0';
if(strcmp(command, "exit")==0)
break;
printf("the command entered: %s \n", command);
}
return 0;
}

