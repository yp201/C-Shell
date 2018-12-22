#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
int cd(char **arguments,int count)
{
	char destination_directory[PATH_MAX+1];
	extern char home_directory_path[PATH_MAX+1];
	if(count>2)
	{
		fprintf(stderr, "cd: too many arguments\n");
		return -1;
	}
	if(count==1)
	{
		strcpy(destination_directory,home_directory_path);	
	}
	else if(count==2 && (strcmp(arguments[1],"~")==0) )
	{
		strcpy(destination_directory,home_directory_path);
	}
	else if(count==2 && (arguments[1][0]=='~'))
	{
		strcpy(destination_directory,home_directory_path);
		strcat(destination_directory,&(arguments[1][1]));
	}
	else if(count==2 && strcmp(arguments[1],".")==0)
	{
		return 1;
	}
	else 
	{
		strcpy(destination_directory,arguments[1]);
	}
	// printf("%s\n",destination_directory );
	int error=chdir(destination_directory);
	// printf("----\n");
	// printf("%d\n",error );
	if(error==-1)
	{
		perror("cd ");
		return 1;
	}
	return 1;
}