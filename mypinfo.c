#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int pinfo(char **arguments,int count)
{
	int pid;
	char path1[32768+20],path2[32768+20],executable_path[PATH_MAX+1];
	char buffer[1024*100];
	if(count>2)
	{
		perror("pinfo: Invalid usage, too many arguments");
		return -1;
	}
	else if(count==1)
	{
		pid=getpid();
	}
	else if(count==2)
	{
		pid=atol(arguments[1]);

	}
	sprintf(path1,"/proc/%d/stat",pid);
	sprintf(path2,"/proc/%d/exe",pid);
	int file_descriptor=open(path1,O_RDONLY);
	if(file_descriptor < 0 )
	{
		fprintf(stderr, "pid: invalid process id\n");
		return -1;
	}
	read(file_descriptor,buffer,1024*100);
	char *info=NULL;
	info=strtok(buffer," ");
	int index=0;
	char status[3];
	unsigned int vm;
	while(info!=NULL)
	{
		index++;
		info=strtok(NULL," ");
		if(index==3)strcpy(status,info);
		if(index==23)
		{
			vm=(unsigned int)info;
			break;
		}
	}

	printf("pid -- %d\n",pid);
	printf("Process status -- %s\n",status);
	printf("Virtual Memory -- %u\n",vm);
	if(readlink(path2,executable_path,(PATH_MAX+1))==-1)
	{
		fprintf(stderr,"Executable Path -- readlink :error while reading \n");
	}
	else
	{
		printf("Executable Path -- %s\n",executable_path);
	}
	return 1;

}