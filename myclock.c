#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int clock_pid=0;
int clock(char **arguments,int count)
{
	int i=1;
	if(count !=3)
	{
		fprintf(stderr, "clock error: standard usage: clock -t <second>\n" );
		return -1;
	}
	if(strcmp(arguments[1],"-t")!=0)
	{
		fprintf(stderr, "clock error: standard usage: clock -t <second>\n" );
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
		while(1)
		{
			sleep((unsigned int)(atoi(arguments[2])));
		
			FILE* rtc = fopen("/proc/driver/rtc","r");
			char time_string[200];
			char date_string[200];
			
			fscanf(rtc,"%s",time_string);
			fscanf(rtc,"%s",time_string);
			fscanf(rtc,"%s",time_string);

			fscanf(rtc,"%s",date_string);
			fscanf(rtc,"%s",date_string);
			fscanf(rtc,"%s",date_string);

			printf("%s, ",date_string );
			printf("%s\n",time_string );
		
		}
	}
	else 
	{
		clock_pid=process_id;
		return 1;
	}

}