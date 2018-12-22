#include <execute.h>
#include <display.h>
#include <input.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <jobcontroller.h>
#include <signal.h>


// void clock_handler()
// {
// 	extern int clock_pid;
// 	if(clock_pid)
// 	{
// 		kill(clock_pid,SIGKILL);
// 		printf("clock with PID %d exited \n",clock_pid);
// 		clock_pid=0;
// 	}

// 	return ;
// }


int main()
{
	init_display();
	init_job_controls();
	while(1)
	{
		fflush(stdout);
		fflush(stderr);

		// signal(SIGINT,clock_handler);

		char **commands=NULL;
		char *display_string = NULL;
		show_display(&display_string);
		getCommand(display_string,&commands);
		int command=0;
		while(1)
		{
			if(commands[command]==NULL)break;
			execute_commands(commands[command]);

			command++;
		}
		if(commands[command])free(commands[command]);
		if(commands)free(commands);
		if(display_string)free(display_string);
	}
	return 0;
}
