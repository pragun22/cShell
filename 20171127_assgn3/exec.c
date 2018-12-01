#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/wait.h>
extern int ppid;
extern int back_size ;
extern int back_order[4000];
extern char back_process[400000+1][300];
extern void redirection(char* arg[],int n);
extern void chandler(int signum);
extern void push(int sig);
extern int flagz;
extern int flagc;
int exec(char * arg[],int n)
{
  pid_t pid;
  int status ;
  int background = 0;
  // printf("%c\n",arg[n-1][0] );
  if(arg[n-1][0] == '&')
  {
  	background = 1;
  	arg[n-1] = NULL;
  }
  if((pid = fork())<0)
  {
    printf("forking failed\n");
    return 0;
  }
  if(pid ==0)
  {
    if(execvp(arg[0],arg)<0)
    {
      printf("Enter a proper command\n");
      exit(0);
    }
  }
  else
  {
    if(background)
    {
    	setpgid(pid,pid);
    	signal(SIGCHLD,SIG_IGN);
      	int i;
    	strcpy(back_process[pid],arg[0]);
      	for (i = 0; i < back_size; ++i)
      	{
      	  if(back_process[back_order[i]][0]=='\0') 
      	  {
      	    back_order[i] = pid;
      	    break;
      	  }
      	}
    	if(i == back_size) back_order[back_size++] = pid;
    	printf("[%d] %d\n",i+1, pid);
    	signal(SIGCHLD, SIG_IGN);
    }
    else
    {
		flagz = 0;
		flagc = 0;
		signal(SIGTSTP, push);
		signal(SIGINT, chandler);
	    while(flagc==0 && flagz==0 && pid != waitpid(pid,&status,WNOHANG) );
	    if(flagc){
	    	kill(pid,9);
	    	wait(NULL);
	    }
	    if(flagz){
	    	setpgid(pid,pid);
	    	kill(pid,SIGSTOP);
	    	int i;
    		strcpy(back_process[pid],arg[0]);
      		for (i = 0; i < back_size; ++i)
      		{
      		  if(back_process[back_order[i]][0]=='\0') 
      		  {
      		    back_order[i] = pid;
      		    break;
      		  }
      		}
    		if(i == back_size) back_order[back_size++] = pid;
    		printf("[%d] %d\n",i+1, pid);
    		signal(SIGCHLD, SIG_IGN);
	    }
	    flagc = 0;
	    flagz =0;
	}

  }
}
int checkpipes(char* arg[],int p)
{
	// printf("%s-=-\n", arg[p]);
	for(int i=0; i < strlen(arg[p]) ; i++)
	{
		// printf("%c\n", arg[p][i]);
		if (arg[p][i] == '|') return 1;
	}
	return 0;
}
char ** getargs(char * cmd) 
{
    // assumes that cmd ends with NULL
    char** argsarray;
    int nargs = 0;
    int nlen = strlen(cmd);
    int i = 0;
    argsarray = (char**) malloc(sizeof(char*) * 256);
    argsarray[0] = strtok(cmd," ");
    i = 0;
    while (argsarray[i] != NULL){
        i++;
        argsarray[i] = strtok(NULL," ");
    }
    return argsarray;
}

void execute_with_pipes(char * arg[], int n)
{

  int fd[256][2];
  char * sepCmd[256];
  char * pch;
  int itr =0;
  int in = 0,out = 0;
  while(arg[n][itr]!='\0')
  {
  	if(arg[n][itr]=='<') in =1;
  	if(arg[n][itr]=='>') out =1;
  	itr++;
  }
  pch = strtok (arg[n], "|");


  int count = 0;  
  while (pch != NULL) 
  {
	// printf("%s\n", pch);
	sepCmd[count] = pch;
	// printf("The value in this array value is: %s\n", sepCmd[count]);
	pch = strtok (NULL, "|");
	count++;
  }

  char ** argue;
  int k;
  k = 0;
  pipe(fd[k]);
  pid_t pid = fork();
  if(pid<0)
  {
  	perror("fork error");
  	exit(0);
  }
  else if(pid==0) 
  {
    dup2(fd[k][1], STDOUT_FILENO);
    close(fd[k][0]);
    argue = getargs(sepCmd[k]);
    if(in){ 
    redirection(argue,5);
	}
    else{
    execvp(argue[0], argue);
    perror(argue[0]);
	}
    exit(0);
  }
  else if(pid>0)
  {
  	  waitpid(pid,0,0);	
	  for(k = 1; k < count - 1; k++) 
	  {
	      close(fd[k-1][1]);
	      pipe(fd[k]);
	      if(!fork()) 
	      {
	          close(fd[k][0]);
	          dup2(fd[k-1][0], STDIN_FILENO);
	          dup2(fd[k][1], STDOUT_FILENO);
	          argue = getargs(sepCmd[k]);
	          execvp(argue[0], argue);
	          perror(argue[0]);
	          exit(0);
	      }
	      else
	      {
	      	while(waitpid(-1, NULL, 0) != -1);
	      }
	  }
	  close(fd[k-1][1]);
	  if(!fork()) 
	  {
	  	  close(fd[k][0]);
	      dup2(fd[k-1][0], STDIN_FILENO);
	      argue = getargs(sepCmd[k]);
	      if(out){
	      	redirection(argue,5);
	      }
	      else{
	      execvp(argue[0], argue);
	      perror(argue[0]);
	      }
	      exit(0);
	  }
	  else
	  {
	  	while(waitpid(-1, NULL, 0) != -1);
	  }
  }
}
