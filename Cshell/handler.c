#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <sys/wait.h> 
#include <signal.h>
extern int ppid;
extern int back_size ;
extern int back_order[4000];
extern char back_process[400000+1][300];
extern void redirection(char* arg[],int n);
extern int flagz;
extern int flagc;
void handler()
{
	pid_t pid;
  pid = wait(NULL);
  if(pid<0)
  {
  }
  else
  {
    printf("\n%s with pid %d exited normally\n", back_process[pid], pid);
    back_process[pid][0] = '\0';
    back_size--;
  }
}

void background_check()
{
  int status;
  int i;
  int pid;
  for (i = 0; i < back_size; ++i)
  {
    pid = back_order[i];
    if(waitpid(pid, &status,WNOHANG) )
    {
      if(back_process[pid][0]!='\0') printf("\n[%d] %s with pid [%d] exited normally\n ",i+1, back_process[pid], pid);
      back_process[pid][0] = '\0';
    }  
  }
}
void chandler(int signum)
{
flagc = 1;
}

void push(int signum)
{
  flagz = 1;
}
