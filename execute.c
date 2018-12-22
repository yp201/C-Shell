#include <unistd.h>
#include <display.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <myecho.h>
#include <mypwd.h>
#include <mycd.h>
#include <mypinfo.h>
#include <myls.h>
#include <myremindme.h>
#include <myclock.h>
#include <jobcontroller.h>

typedef struct 
{
	int write;
	char** command_arguments;
	char* input_file;
	char* output_file;

}command_object;


struct builtin_commands{
	int (*execute_command)(char **arguments, int count);
	char *command;
} builtins[] = {
				{pwd,"pwd"},{echo,"echo"},{cd,"cd"},{pinfo,"pinfo"},
				{ls,"ls"},{remindme,"remindme"},{clock,"clock"},
				{print_jobs,"jobs"},{kjob,"kjob"},{fg,"fg"},{bg,"bg"},
				{mysetenv,"setenv"},{myunsetenv,"unsetenv"},{overkill,"overkill"},
				{quit,"quit"}
			};


int no_of_builtins=sizeof(builtins)/sizeof(builtins[1]);

extern char home_directory_path[PATH_MAX+1];

int tilde_to_home(char *dest, char *argument) 
{
	if(strcmp(argument, "~") == 0) 
	{
		strcpy(dest, home_directory_path);
	}
	else if(argument[0] == '~' && argument[1] == '/') 
	{
		strcpy(dest, home_directory_path);
		strcat(dest, &argument[1]);
	}
	else
	{
		strcpy(dest, argument) ; 
	}
	return 0;
}

int command_parser(command_object** command_list,char* commands)
{
	int COMMAND_SIZE = 400;
	int current_command_size=COMMAND_SIZE;
	int index=0,i=0;
	*command_list = malloc(sizeof(command_object) * current_command_size);
	char* command;
	command =strtok(commands,"|");
	while(command !=NULL)
	{
		(*command_list)[index].input_file= command;
		index++;
		if(current_command_size == index)
		{
			(*command_list) = realloc((*command_list), (current_command_size+COMMAND_SIZE) * sizeof(command_object));
			current_command_size+=COMMAND_SIZE;
		}
		command = strtok(NULL, "|");

	}
	(*command_list)[index].write=0;
	(*command_list)[index].output_file='\0';
	(*command_list)[index].command_arguments='\0';
	(*command_list)[index].input_file='\0';
	int position;
	char *arguments;
	// printf("%d\n",index );
	for(i=0;i<index;i++)
	{
		char* argument;
		position=0;
		current_command_size=COMMAND_SIZE;
		arguments=(*command_list)[i].input_file;
		(*command_list)[i].write=0;
		(*command_list)[i].output_file=NULL;
		(*command_list)[i].command_arguments= malloc(current_command_size * sizeof(char*));
		(*command_list)[i].input_file=NULL;
		
		argument = strtok(arguments, " \t");
		while(argument !=NULL)
		{
			if(strcmp(argument,"<")==0)
			{
				argument = strtok(NULL, " \t");
				if(argument!=NULL)
				{
					(*command_list)[i].input_file=argument;
				}
				else
				{
					fprintf(stderr, "Wrong syntax,specify a input file after '<'\n");
					return -1;
				}
			}
			else if(strcmp(argument,">")==0)
			{
				argument = strtok(NULL, " \t");
				if(argument!=NULL)
				{
					(*command_list)[i].output_file=argument;
					(*command_list)[i].write=0;
				}
				else
				{
					fprintf(stderr, "Wrong syntax,specify a output file after '>'\n");
					return -1;
				}

			}
			else if(strcmp(argument,">>")==0)
			{
				argument = strtok(NULL, " \t");
				if(argument!=NULL)
				{
					(*command_list)[i].output_file=argument;
					(*command_list)[i].write=1;
				}
				else
				{
					fprintf(stderr, "Wrong syntax,specify a output file after '>>'\n");
					return -1;
				}

			}
			else
			{
				// (*command_list)[i].command_arguments[position]=malloc(sizeof(char) * PATH_MAX);
				(*command_list)[i].command_arguments[position] = argument;
				position++;
				if(current_command_size == position)
				{
					(*command_list)[i].command_arguments = realloc((*command_list)[i].command_arguments, (current_command_size+COMMAND_SIZE) * sizeof(char*));
					current_command_size+=COMMAND_SIZE;
				}
			}
			argument = strtok(NULL, " \t");
		}
		(*command_list)[i].command_arguments[position]=NULL;
		int j=0;
		char *temp_argument=NULL;
		while(j<position)
		{
			temp_argument=(*command_list)[i].command_arguments[j];
			(*command_list)[i].command_arguments[j] = malloc(PATH_MAX * sizeof(char)  );
			tilde_to_home((*command_list)[i].command_arguments[j], temp_argument);
			j++;
		}
		
		if((*command_list)[i].output_file) 
		{
			temp_argument = (*command_list)[i].output_file;
			(*command_list)[i].output_file = malloc(PATH_MAX * sizeof(char) );
			tilde_to_home((*command_list)[i].output_file, temp_argument);
		}
		if((*command_list)[i].input_file) 
		{
			temp_argument = (*command_list)[i].input_file;
			(*command_list)[i].input_file = malloc(PATH_MAX * sizeof(char) );
			tilde_to_home((*command_list)[i].input_file, temp_argument);
		}
	}

	return 0;

}

int execute_commands(char *command)
{
	int background_process=0;
	int command_length=strlen(command);
	if(command_length > 1 && command[command_length-1]=='&')
	{
		command_length--;
		command[command_length]=NULL;
		background_process=1;
	}
	// printf("%d\n",background_process );
	command_object *command_list=NULL;
	if (command_parser(&command_list,command) == -1) return -1;
	int total_commands=0;
	int save_input=dup(0),save_output=dup(1),input=0;
	while(command_list[total_commands].command_arguments)
	{
		total_commands++;
	}
	// printf("***********\n");
	// printf("%d\n",total_commands );
	// printf("***********\n");
	int i=0;
	while(command_list[i].command_arguments!=NULL)
	{
		// printf("==============\n");
		// printf("%d\n",i );
		// printf("%s\n",command_list[i].command_arguments[0]);
		// printf("==============\n");
		int piping[2];
		if(pipe(piping)!=0)
		{
			perror("piping failed");
		}
		int total_arguments=0;
		int built_in=0,j=0;
		for(j=0;j<no_of_builtins;j++)
		{
			if(strcmp(builtins[j].command,command_list[i].command_arguments[0])==0)
			{
				built_in=1;
				while(command_list[i].command_arguments[total_arguments])total_arguments++;

				int out;
				if(total_commands==1)
				{
					if(command_list[i].output_file)
					{
						if(!command_list[i].write)
						{
							out=open(command_list[i].output_file, O_WRONLY | O_CREAT | O_TRUNC, (S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR) );
						}
						else
						{
							out=open(command_list[i].output_file,  O_WRONLY | O_APPEND | O_CREAT ,(S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR));
						}
						if(out==-1)
						{
							fprintf(stderr, "Error in opening output file\n");
							return -1;
						}
						if(dup2(out,STDOUT_FILENO)!=1)
						{
							perror("Error,dup2 failed");
						}
						close(out);
					}
					if(command_list[i].input_file)
					{
						int inp;
						inp=open(command_list[i].input_file,O_RDONLY);
						if(inp==-1)
						{
							fprintf(stderr, "Error in opening input file\n");
							return -1;
						}
						if(dup2(inp,STDIN_FILENO)!=1)
						{
							perror("Error,dup2 failed");
							return -1;
						}
						close(inp);
					}
					int error=builtins[j].execute_command(command_list[i].command_arguments,total_arguments);
					if(error==-1)return -1;
					dup2(save_input,0);
					close(save_input);
					dup2(save_output,1);
					close(save_output);
				}
				else
				{
					pid_t process_id=fork();
					if(process_id == -1)
					{
						perror("Error while fork");
						return -1;
					}
					else if(process_id == 0)
					{
						if( (command_list[i+1].command_arguments) && i==0 && input == 0 )
						{
							dup2(piping[1],STDOUT_FILENO);
						}
						else if( (command_list[i+1].command_arguments) && i!=0 && input!=0 )
						{
							dup2(piping[1],STDOUT_FILENO);
							dup2(input,STDIN_FILENO);
						}
						else
						{
							dup2(input,STDIN_FILENO);
						}
						if(command_list[i].output_file)
						{
							int out;
							if(!command_list[i].write)
							{
								out=open(command_list[i].output_file,  O_WRONLY | O_CREAT | O_TRUNC  , (S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR));
							}
							else
							{
								out=open(command_list[i].output_file, O_WRONLY | O_APPEND | O_CREAT, (S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR));
							}
							if(out==-1)
							{
								fprintf(stderr, "Error in opening output file\n");
								return -1;
							}
							if(dup2(out,STDOUT_FILENO)!=1)
							{
								perror("Error,dup2 failed");
							}
							close(out);
						}
						if(command_list[i].input_file)
						{
							int inp;
							inp=open(command_list[i].input_file,O_RDONLY);
							if(inp==-1)
							{
								fprintf(stderr, "Error in opening input file\n");
								return -1;
							}
							if(dup2(inp,STDIN_FILENO)!=1)
							{
								perror("Error,dup2 failed");
								return -1;
							}
							close(inp);
						}
						int error=builtins[j].execute_command(command_list[i].command_arguments,total_arguments);
						if(error==-1)return-1;
						exit(0);
					}
					else
					{
						wait(NULL);
					}
					close(piping[1]);
					if(command_list[i+1].command_arguments == NULL)
					{
						close(piping[0]);
					}
					if(input!=0)
					{
						close(input);
					}
					input=piping[0];

				}
			}

		}
		if(built_in == 0)
		{
			pid_t process_id=fork();
			if(process_id == -1)
			{
				perror("Error while fork");
				return -1;
			}
			else if(process_id == 0)
			{
				if(i==0 && input == 0 && command_list[i+1].command_arguments!=NULL)
				{
					dup2(piping[1],STDOUT_FILENO);
				}
				else if(i!=0 && input!=0 && command_list[i+1].command_arguments!=NULL)
				{
					dup2(piping[1],STDOUT_FILENO);
					dup2(input,STDIN_FILENO);
				}
				else
				{
					dup2(input,STDIN_FILENO);
				}
				if(command_list[i].output_file)
				{
					int out;
					if(!command_list[i].write)
					{
						out=open(command_list[i].output_file, O_WRONLY | O_CREAT | O_TRUNC,(S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR)  );
					}
					else
					{
						out=open(command_list[i].output_file,  O_WRONLY | O_APPEND | O_CREAT, (S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR));
					}
					if(out==-1)
					{
						fprintf(stderr, "Error in opening output file\n");
						exit(0);
						return -1;
					}
					if(dup2(out,STDOUT_FILENO)==-1)
					{
						exit(0);
						perror("Error,dup2 failed");
					}
					close(out);
				}
				if(command_list[i].input_file)
				{
					int inp;
					inp=open(command_list[i].input_file,O_RDONLY);
					if(inp==-1)
					{
						fprintf(stderr, "Error in opening input file\n" );
						exit(0);
						return -1;
					}
					if(dup2(inp,STDIN_FILENO)==-1)
					{
						perror("Error,dup2 failed");
						exit(0);
						return -1;
					}
					close(inp);
				}
				setpgid(0,0);
				execvp(command_list[i].command_arguments[0],command_list[i].command_arguments);
				perror("command not found");
				exit(0);
			}
			else
			{
				if(!background_process)
				{
					siginfo_t fg_status;
					add_to_foreground(process_id,command_list[i].command_arguments[0]);
					tcsetpgrp(STDIN_FILENO,process_id);
					waitid(P_PID,process_id,&fg_status,(WUNTRACED | WNOWAIT));
				}
				else
				{
					add_to_background(process_id,command_list[i].command_arguments[0]);
				}
			}
			close(piping[1]);
			if(command_list[i+1].command_arguments==NULL)
			{
				close(piping[0]);
			}
			if(input!=0)
			{
				close(input);
			}		
			input=piping[0];
		}
		i++;
		// printf("%d\n",i );
	}
	dup2(save_input, 0);
	close(save_input);
	dup2(save_output, 1);
	close(save_output);

	// printf("here\n");

	i=0;

	while(command_list[i].command_arguments!=NULL)
	{
		int j=0;
		while(command_list[i].command_arguments[j]!=NULL)
		{
			free(command_list[i].command_arguments[j]);
			j++;
		}
		free(command_list[i].command_arguments);
		if(command_list[i].output_file)
		{
			free(command_list[i].output_file);
		}
		if(command_list[i].input_file)
		{
			free(command_list[i].input_file);
		}
		i++;
	}
	free(command_list);
	return 1;
}