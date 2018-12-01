#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
extern int ppid;
extern char message[1024];
extern int in;
extern int out;
extern int back_size ;
extern int back_order[4000];
extern char back_process[400000+1][300];
extern void redirection(char* arg[],int n);
void check_redirection(char* arg[])
{
	int i = 0;
	while(arg[i]!=NULL)
	{
		if(!strcmp(arg[i],">") || !strcmp(arg[i],">>")) in++;
		if(!strcmp(arg[i],"<")) out++;	
		i++;
	}
}

void redirection(char* arg[],int n)
{
	if(in && out)
	{
		FILE* fd1;
		FILE* fd2;
		pid_t pid;
		for (int i = 0; i < n; ++i)
		 {
		 	if(!strcmp(arg[i],"<"))
		 	{
		 		arg[i] = NULL;
		 		pid = fork();
		 		if(pid<0)
		 		{
		 			perror("forking failed");
		 			return ;
		 		}
		 		if(pid==0)
		 		{
					fd1 = fopen(arg[i+1],"r");
					arg[i+1] = NULL;
					fd2 = fopen(arg[n-1],"w");
					arg[n-1] = NULL;
					arg[n-2] = NULL;
					dup2(fileno(fd1),STDIN_FILENO);
					dup2(fileno(fd2),STDOUT_FILENO);
					fclose(fd1);
					fclose(fd2);
					execvp(arg[0], arg);
					perror(arg[0]);
					exit(0);
				}
				else
				{
					waitpid(pid, 0, 0);
				}	
		 	}
		 } 
		 return ;
	}
	int cmd = 1;
	int i;
	for (i = 0; i < n; ++i)
	{
		pid_t pid ;
		FILE* fd;
		if(arg[i]!=NULL)
		{
			if(!strcmp(arg[i],">") || !strcmp(arg[i],">>"))
			{
				cmd = 0;
				int app = 0;
				if(!strcmp(arg[i],">>")) app =1;
				arg[i] = NULL;
				// fclose(fd);
				pid = fork();
				if(pid<0) 
				{
					perror("forking failed");
					return;
				}
				if(pid==0)
				{
					if(!app)fd = fopen(arg[i+1], "w");
					else fd = fopen(arg[i+1],"a");
					arg[i+1] = NULL;
					dup2(fileno(fd), STDOUT_FILENO);
					fclose(fd);
					execvp(arg[0],arg);
					perror(arg[0]);
					exit(0);
				}	
				else
				{
					int return_status;
					waitpid(pid, &return_status, 0);
				}

			}
			else if(!strcmp(arg[i],"<"))
			{
				if(out>1) 
				{
					perror("wrong syntax");
					return ;
				}
				arg[i] = NULL;
				pid = fork();
				if(pid<0)
				{
					printf("forking failed\n");
					return;
				}
				if(pid==0)
				{
					fd = fopen(arg[i+1], "r");
					if(fd<0)
					{
						perror("could not open file");
						exit(0);
					}
					arg[i+1] = NULL;
					dup2(fileno(fd), STDIN_FILENO);
					fclose(fd);
					execvp(arg[0], arg);
					exit(0);
				}
				else
				{
					int return_status;
					waitpid(pid, &return_status, 0);
				}
			}
		}
	}
}
// void redirection(char* arg[], int n)
// {
// 	char * temp[1024];
// 	int cmd = 1;
// 	int i;
// 	int status;	
// 	for (i = 0; i < n; ++i)
// 	{
// 		int pid = fork();
// 		if(pid<0) 
// 		{
// 			perror("forking failed");
// 			return;
// 		}
// 		if(pid == 0)
// 		{
// 				FILE* fd;
// 				if(arg[i]!=NULL)
// 				{
// 					if(!strcmp(arg[i],">"))
// 					{
// 						cmd = 0;
// 						arg[i] = NULL;
// 						fd = fopen(arg[i+1], "w");
// 						arg[i+1] = NULL;
// 						dup2(fileno(fd),1);
// 						fclose(fd);
// 						execvp(temp[0],temp);
// 						exit(0);
// 					}
// 				}
// 				if(cmd) temp[i] = arg[i];	
// 		}
// 		else
// 		{
// 			waitpid(pid, 0, 0);
// 		}
// 	}
// }