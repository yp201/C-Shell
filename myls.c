#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char home_directory_path[PATH_MAX+1];

long long int calculate_total_block_size(char *directory_path,int flag[])
{
	long long int size =0;
	DIR *directory;
	directory=opendir(directory_path);
	struct dirent *current_file;
	if(directory==NULL)
	{
		perror("ls: opendir error");
		return -1;
	}
	current_file = readdir(directory);
	while(current_file!=NULL)
	{
		struct stat file;
		char file_path[PATH_MAX+1];
		strcpy(file_path,directory_path);
		strcat(file_path,"/");
		strcat(file_path,current_file->d_name);	
		stat(file_path,&file);
		if((current_file->d_name)[0]=='.')
		{
			if(flag[0])
			{
				size+=file.st_blocks ;
			}
		}
		else
		{
			size+=file.st_blocks ;
		}
		current_file=readdir(directory);
	}
	closedir(directory);
	return size;
}

int get_permissions(struct stat file,char* permissions)
{
    int i;
    
    for(i=0;i<10;i++) permissions[i] = '-';
    if(S_ISLNK(file.st_mode)) permissions[0]='l';
    else if(S_ISDIR(file.st_mode)) permissions[0]='d';
	else if(S_ISCHR(file.st_mode)) permissions[0]='c';
	else if(S_ISFIFO(file.st_mode)) permissions[0]='p';
	else if(S_ISSOCK(file.st_mode)) permissions[0]='s';
	else if(S_ISBLK(file.st_mode)) permissions[0]='b';
    
    if(file.st_mode & S_IRUSR) permissions[1]='r';
    if(file.st_mode & S_IWUSR) permissions[2]='w';
    if(file.st_mode & S_IXUSR) permissions[3]='x';

    if(file.st_mode & S_IRGRP) permissions[4]='r';
    if(file.st_mode & S_IWGRP) permissions[5]='w';
    if(file.st_mode & S_IXGRP) permissions[6]='x';

    if(file.st_mode & S_IROTH) permissions[7]='r';
    if(file.st_mode & S_IWOTH) permissions[8]='w';
    if(file.st_mode & S_IXOTH) permissions[9]='x';
   
    return 1;
    
}

int print_file(char* file_path,int l,char* name,long long int size)
{
	struct stat file;
	int error=stat(file_path,&file);
	if(error==-1)
	{
		perror("stat: no such file or directory");
		return -1;
	}
	if(l)
	{
		
		char permissions[11];
		get_permissions(file,permissions);
		if(size!=-1)printf("total %lld\n",(size)/2);
		printf("%s ",permissions);
		printf("%3lld ", (long long)file.st_nlink);
		printf("%10s ", getpwuid(file.st_uid)->pw_name);
		printf("%10s ", getgrgid(file.st_gid)->gr_name);
		printf("%10lld ", (long long)file.st_size);

		// char *c_time_string = ctime(&file.st_mtim);
		char date[400];
		strftime(date, 20, "%b %d %R", localtime(&(file.st_mtim)) );
		printf("%s ",date );
		// c_time_string[strlen(c_time_string) - 1] = '\0';
		// printf("%s ", c_time_string);
	}

	if(S_ISCHR(file.st_mode)) printf("\033[40;33;1m");
	else if(S_ISLNK(file.st_mode)) printf("\033[01;36m");
	else if(S_ISFIFO(file.st_mode)) printf("\033[40;33m");
	else if(S_ISDIR(file.st_mode)) printf("\033[1;34m");
	else if(S_ISSOCK(file.st_mode)) printf("\033[01;35m");
	else if(S_ISBLK(file.st_mode)) printf("\033[40;33;1m");
	else if(file.st_mode & S_IXUSR) printf("\033[32;1m");

	// if(strlen(fileName)) printf("%s\033[0m", fileName);
	if(strstr(name, "/") == NULL) printf("%s\033[0m", name);
	else printf("%s\033[0m", 1+strrchr(name, (int)'/'));
	printf("\n");
	return 1;
}

int open_dir(char *directory_path,int flag[],long long int size)
{
	DIR *directory;
	directory=opendir(directory_path);
	struct dirent *current_file;
	if(directory==NULL)
	{
		perror("ls: opendir error");
		return -1;
	}
	current_file = readdir(directory);
	int first=1;
	long long int total=-1;
	while(current_file!=NULL)
	{
		total=-1;
		char file_path[PATH_MAX+1];
		strcpy(file_path,directory_path);
		strcat(file_path,"/");
		strcat(file_path,current_file->d_name);	
		if(first)
		{
			total=size;
			first=0;
		}
		if((current_file->d_name)[0]=='.')
		{
			if(flag[0])
			{
				print_file(file_path,flag[1],current_file->d_name,total);
			}
		}
		else
		{
			print_file(file_path,flag[1],current_file->d_name,total);
		}
		current_file=readdir(directory);
	}
	closedir(directory);
	return 1;

}

int show_list(char *path,int flag[])
{
	char directory_path[PATH_MAX+1];
	if(strcmp(path,"~")==0)
	{
		strcpy(directory_path,home_directory_path);
	}
	else if(path[0]=='~' && path[1]=='/')
	{
		strcpy(directory_path,home_directory_path);
		strcat(directory_path,&path[1]);
	}
	else
	{
		strcpy(directory_path,path);
	}
	struct stat dir;
	int error=stat(directory_path,&dir);
	if(error==-1)
	{
		perror("ls: no such file or directory");
		return -1;
	}
	if ((S_ISDIR(dir.st_mode)))
	{
		long long int size=calculate_total_block_size(directory_path,flag);
		int error=open_dir(directory_path,flag,size);
		if(error == -1)
		{
			return -1;
		}
	}
	else 
	{
		int error=print_file(directory_path,flag[1],directory_path,-1);
		if(error==-1)
		{
			return -1;
		}

	}

	return 1;
}


int ls(char **arguments,int count)
{
	int flag[2]={0};
	int index=1;
	while(arguments[index]!=NULL)
	{
		if(arguments[index][0]=='-')
		{
			int len=strlen(arguments[index]);
			int j=1,valid=0;
			for(j=1;j<len;j++)
			{
				if(arguments[index][j]=='l')
				{
					flag[1]=1;
					valid=1;
				}
				else if(arguments[index][j]=='a')
				{
					flag[0]=1;
					valid=1;
				}
			}
			if(!valid)
			{
				fprintf(stderr, " ls: invalid option -- 'e'\n valid flags: 'l' and 'a' ");
				return -1;
			}
		}
		index++;
	}
	index=1;
	int directories=0;
	while(arguments[index]!=NULL)
	{
		if(arguments[index][0]!='-')
		{
			directories++;
		}
		index++;
	}
	index=1;
	int temp=directories;
	while(arguments[index]!=NULL)
	{
		if(arguments[index][0]!='-')
		{
			if(directories>1)
			{
				printf("%s:\n",arguments[index]);
			}
			show_list(arguments[index],flag);
			temp--;
			if(temp>0)printf("\n");
		}
		index++;
	}

	if(directories == 0)
	{
		char present_working_directory[PATH_MAX+1];
		getcwd(present_working_directory, PATH_MAX + 1);
		show_list(present_working_directory,flag);
	}
	return 1;
}