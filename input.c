#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execute.h>
#include <readline/readline.h>
#include <readline/history.h>


int getCommand(char *display_string,char ***commands)
{
	int COMMAND_SIZE=400;
	int current_command_size=COMMAND_SIZE;
	int index=0;
	(*commands) = malloc(sizeof(char*) * current_command_size);
	if((*commands) ==NULL)
	{
		perror("malloc:error while malloc");
		return -1;
	}
	char *input=NULL;
	char *token=NULL;
	input = readline(display_string);
	if(input) add_history(input);
	if(input) token = strtok(input, ";");
	while(token !=NULL)
	{
		if(token!=NULL)(*commands)[index]=token;
		if(index==current_command_size)
		{
			(*commands) = realloc((*commands),sizeof(char*) * (current_command_size +COMMAND_SIZE));
			current_command_size+=COMMAND_SIZE;
		}
		token = strtok(NULL, ";");
		index++;
	}
	(*commands)[index]=NULL;
	return 0;	
}