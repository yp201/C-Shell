#include <unistd.h>
#include <display.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <jobcontroller.h>
#include <signal.h>

typedef struct background_command
{
	char* command;
	int index;
	pid_t process_id;
	struct background_command* next_command;
	struct background_command* prev_command;

} background_commands;


background_commands* foreground_process = NULL;
background_commands* start_background_process=NULL;

void sigint_handler()
{
	if(foreground_process)kill(foreground_process->process_id,SIGINT);

}
void sigtstp_handler()
{
	if(foreground_process)kill(foreground_process->process_id,SIGTSTP);
}
void child_handler()
{
	pid_t process_id;
	int status;
	while((process_id = waitpid(-1, &status, (WNOHANG | WUNTRACED)))> 0)
	{
		// process_id=waitpid(-1, &status, (WNOHANG | WUNTRACED));
		background_commands* itr=start_background_process;
		background_commands* prev=NULL;
			// printf("level1\n");

		while(itr!=NULL)
		{
			// printf("level2\n");

			if(itr->process_id==process_id)
			{
				// printf("come\n");
				if(!prev)
				{
					start_background_process=itr->next_command;
				}
				else
				{
					prev->next_command=itr->next_command;
				}
				break;
			}
			prev=itr;
			itr=itr->next_command;
		}
		// printf("level3\n");
		if(foreground_process!=NULL && foreground_process->process_id==process_id   )
		{
			tcsetpgrp(STDIN_FILENO, getpgid(0));
			(WIFEXITED(status));
			if(WIFSTOPPED(status))
			{
				printf("%s with pid %d was stopped\n", foreground_process->command,foreground_process->process_id );
				add_to_background(foreground_process->process_id,foreground_process->command);
			}
			else if(WIFSIGNALED(status))
			{
				printf("%s with pid %d was terminated\n",foreground_process->command,foreground_process->process_id);
			}
			free(foreground_process->command);
			free(foreground_process);
			foreground_process=NULL;
		}
		else
		{
		
			int stopped=0;
			if(WIFEXITED(status))
			{
				int exit_status=WIFEXITED(status);
				printf("%s with pid %d exited with status %d \n", itr->command,process_id,exit_status );
			}
			else if(WIFSTOPPED(status))
			{
				printf("%s with pid %d was stopped\n", itr->command,process_id );
				stopped=1;
			}
			else if(WIFSIGNALED(status))
			{
				printf("%s with pid %d was terminated\n",itr->command,process_id);
			}
			if(!stopped)
			{
				free(itr->command);
				free(itr);
			}
		}
	}
}

void init_job_controls() 
{
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sigtstp_handler);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, child_handler);
	return;
}


void recursive_print_job(background_commands* itr) 
{
	if(itr == NULL) 
	{
		return ;
	}
	recursive_print_job(itr->next_command);

	char path_to_process[PATH_MAX+1],path_to_process1[PATH_MAX+1];
	sprintf(path_to_process, "/proc/%d/wchan", itr->process_id);
	sprintf(path_to_process1, "/proc/%d/cmdline", itr->process_id);
	FILE* file_descriptor = fopen(path_to_process, "r");
	FILE* file_descriptor2 = fopen(path_to_process1, "r");
	printf("[%d]\t", itr->index);
	
	if(file_descriptor != NULL)
	{
		char status[PATH_MAX+1];
		fscanf(file_descriptor, "%s", status);
	
		if(strcmp(status, "do_signal_stop") != 0) 
		{
			printf("Running\t");
		}
		else 
		{
			printf("Stopped\t");
		}
		fclose(file_descriptor);
		if(file_descriptor2 != NULL)
		{
			char process_name[PATH_MAX+1];
			fscanf(file_descriptor2, "%s", process_name);

			if(strstr(process_name, "/")) 
			{
				printf("\t%s [%d]\n", 1 + strrchr(process_name, (int)'/'), itr->process_id);
			}
			else 
			{
				printf("\t%s [%d]\n", process_name, itr->process_id);
			}
			fclose(file_descriptor2);
		}
	}
	return ;
}

int print_jobs(char **arguments, int count)
{
	recursive_print_job(start_background_process);
	return 1;
}

int mysetenv(char **arguments, int count)
{
	if(count == 3) 
	{
		setenv(arguments[1], arguments[2], 1);
		return 1;
	}
	else if(count == 2) 
	{
		setenv(arguments[1], "", 1);
		return 1;	
	}
	else 
	{
		fprintf(stderr, "Setenv: Invalid Usage,correct usage: setenv var [value]\n");
		return 1;
	}
	return 1;
}

int myunsetenv(char **arguments, int count)
{
	if (count!=2) 
	{
		fprintf(stderr, "Unsetenv: Invalid Usage, correct usage: unsetenv <var>\n");
		return 0;
	}
	else
	{
		unsetenv(arguments[1]);
		return 1;
	}
}



int overkill(char **arguments, int count)
{
	background_commands *iterator = start_background_process;
	while(iterator!=NULL)
	{
		kill(iterator->process_id, SIGKILL);
		iterator = iterator->next_command;
	}
	return 0;
}

int add_to_background(pid_t process_id, char* command_argument) 
{
	background_commands* current = malloc(sizeof(background_commands));
	if(start_background_process !=NULL) 
	{
		current->index=start_background_process->index + 1;

	}
	else
	{
		current->index = 1;
	} 
	current->command = malloc(sizeof(char) * (1 + strlen(command_argument)));
	current->next_command = start_background_process;
	current->process_id = process_id;
	strcpy(current->command, command_argument);
	start_background_process = current;
	return 1;
}

int add_to_foreground(pid_t process_id, char* command_argument) 
{
	foreground_process = malloc(sizeof(background_commands));
	foreground_process->process_id = process_id;
	foreground_process->next_command = NULL;
	foreground_process->index = -1;
	foreground_process->command = malloc(sizeof(char) * (1 + strlen(command_argument)));
	strcpy(foreground_process->command, command_argument);
	return 1;
}

int quit(char **arguments, int count)
{
	overkill(arguments, count);
	exit(0);
}

int index_changer(background_commands* processes[],int k)
{
	
	int i=0;
	for(i=0;i<k;i++)
	{
		(processes[i])->index-=1;
	}
	return 1;
}

int kjob(char **arguments, int count){
	if(count != 3)
	{
		fprintf(stderr, "Error: Invalid Usage\n Usage: kjob <id> <signal>\n");
		return -1;
	}
	background_commands* iterator = start_background_process;
	background_commands* processes[100];
	int k=0;
	while(iterator!=NULL)
	{
		int id=atoi(arguments[1]);
		int signal=atoi(arguments[2]);
		if(iterator->index == id)
		{
			if(signal == 9 )
			{
				kill(iterator->process_id,signal);
				iterator->index=-1;
				index_changer(processes,k);
				return 0;
			}
			kill(iterator->process_id,signal);
			return 0;
		}
		processes[k]=iterator;
		k++;
		iterator = iterator->next_command;
	}
	fprintf(stderr, "Error: No job with job number %d exists\n", atoi(arguments[1]));
	return -1;
}

int bg(char **arguments, int count)
{
	background_commands *iterator = start_background_process;
	if(count != 2)
	{
		fprintf(stderr, "Error: Invalid Usage\nUsage: bg <job no> \n");
		return 1;
	}
	while(iterator!=NULL)
	{
		if(iterator->index == atoi(arguments[1]))
		{
			kill(iterator->process_id, SIGCONT);
			return 0;
		}
		iterator = iterator->next_command;

	}
	fprintf(stderr, "Error: No job with job number %d exists\n", atoi(arguments[1]));
	return -1;
}

int fg(char **arguments, int count)
{
	if(count != 2)
	{
		fprintf(stderr, "Error: Invalid Usage\nUsage: fg <job id>\n");
		return 1;
	}
	background_commands* processes[100];
	background_commands *iterator = start_background_process, *prev = NULL;
	int k=0;
	while(iterator !=NULL) 
	{
		if(iterator->index == atoi(arguments[1]))
		{
			kill(iterator->process_id, SIGCONT);
			if(prev)
			{
				prev->next_command = iterator->next_command;
			}
			else
			{
				start_background_process = iterator->next_command;
			}
			siginfo_t fgStatus;
			add_to_foreground(iterator->process_id, iterator->command);
			index_changer(processes,k);
			free(iterator->command);
			free(iterator);
			tcsetpgrp(STDIN_FILENO, foreground_process->process_id);
			waitid(P_PID, foreground_process->process_id, &fgStatus, (WUNTRACED | WNOWAIT));

			return 0;
		}
		processes[k]=iterator;
		k++;
		iterator = iterator->next_command;
		prev = iterator;
	}
	fprintf(stderr, "Error: No job with job number %d exists\n", atoi(arguments[1]));
	return 1;
}