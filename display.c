#include <unistd.h>
#include <display.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>

char hostname[HOST_NAME_MAX + 1];
char home_directory_path[PATH_MAX + 1];
uid_t uid;
struct passwd *name;

void init_display()
{

	uid  = geteuid();
	name = getpwuid(uid);
	if (getcwd(home_directory_path, PATH_MAX + 1)==NULL)
	{
		perror("error retrieving home directory");
		exit(0);
	}
	if (gethostname(hostname,HOST_NAME_MAX+1)==-1)
	{
		perror("Host name retrieval failed");
		exit(0);
	}

}

void show_display(char **display_string)
{
	*display_string=malloc(sizeof(char)*(2*HOST_NAME_MAX + PATH_MAX+2));
	memset( *display_string, 0, strlen(*display_string)+1 );
	char current_directory[PATH_MAX+1];
	getcwd(current_directory, PATH_MAX + 1);
	int index=0,flag=1;
	while(home_directory_path[index]!=NULL)
	{
		if(home_directory_path[index] != current_directory[index])
		{
			flag=0;
			break;
		}
		index++;
	}
	sprintf(*display_string+strlen(*display_string),"<\033[0;36m%s\033[0m@\033[0;32m%s\033[0m:\033[1;33m", name->pw_name, hostname);
	(flag? sprintf(*display_string+strlen(*display_string),"~%s\033[0m> ", &current_directory[strlen(home_directory_path)]):sprintf(*display_string+strlen(*display_string),"%s\033[0m> ", current_directory));
	// printf("%s\n",*display_string );
}

