#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
int remindme(char **arguments,int count)
{
	int i=1;
	if(count <2)
	{
		fprintf(stderr, "remindme error: standard usage: remindme <second> <\"message\">\n" );
		return -1;
	}
	if(arguments[2][0]!='"' || arguments[count-1][strlen(arguments[count-1])-1]!='"')
	{
		fprintf(stderr, "remindme error: standard usage: remindme <second> <\"message\">\n" );
		return -1;
	}
	pid_t process_id = fork();
	if(process_id == -1)
	{
		perror("Error while fork");
		return -1;
	}
	else if (process_id == 0)
	{
		sleep((unsigned int)(atoi(arguments[1])));
		printf("Reminder: ");
		for(i=1;i<strlen(arguments[2]);i++)printf("%c",arguments[2][i]);
		for(i=3;i<count-1;i++)printf(" %s",arguments[i]);
		printf(" ");
		for(i=0;i<strlen(arguments[count-1])-1;i++)printf("%c",arguments[count-1][i]);
		printf("\n");
		free(arguments);
		exit(0);
	} 
	else 
	{
		return 1;
	}
}