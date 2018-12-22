#include <unistd.h>
#include <stdio.h>
#include <limits.h>
int pwd(char **arguments,int count)
{
	char present_working_directory[PATH_MAX+1];
	if(getcwd(present_working_directory, PATH_MAX + 1)==NULL)
	{
		perror("error in getcwd while getting the present working directory");
		return -1;
	}
	printf("%s\n",present_working_directory );
	return 1;
}