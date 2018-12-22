#include <stdio.h>
int echo(char **arguments,int count)
{
	if(count==1)
	{
		printf("\n");
		return 1;
	}
	int i=1;
	while(arguments[i+1]!=NULL)
	{
		printf("%s ",arguments[i] );
		i++;
	}
	printf("%s\n",arguments[i] );
	return 1;
}