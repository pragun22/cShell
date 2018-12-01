#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
extern int ppid;
extern char message[1024];
extern int in;
extern int out;
extern int back_size ;
extern int back_order[4000];
extern char back_process[400000+1][300];
extern void redirection(char* arg[],int n);
extern void chandler(int signum);
extern void push(int sig);
extern int flagz;
extern int flagc;
void jobs()
{
	int j;
	for (j = 0; j < back_size; ++j)
	{
		int pid = back_order[j];
	if(back_process[pid][0]!='\0') {			
		int length = 0,c=0;
	  	size_t len = 0;
		char *status = NULL;
	  	char *memory = NULL;
	  	char exec_path[1024] = "";
	  	int i;
	  	char path[1024];
	  	sprintf(path,"/proc/%d/stat", pid);
	  	FILE* f = fopen(path,"r");
	  	getline(&status,&len,f);
	  	for (i = 0; i < 2000; ++i)
	  	{
	  	  if(c==2)
	  	  {
	  	    break;
	 	   }
	 	   if(c==1)
	 	   {
		      length++;
		    }
		    if(status[i]=='(' || status[i]==')') c++;
		  }
		  int temp = pid;
		  int digits = 0;
		  while(temp>0)
		  {
		    temp/=10;
		    digits++;
		  }
		  char st = status[digits+length+3];
		if(st!='T') printf("[%d] Running %s [%d]\n",j+1, back_process[pid], pid);
		else  printf("[%d] stopped %s [%d]\n",j+1, back_process[pid], pid);
		}
	}
}

void kjobs(char* arg[], int n)
{
	int pid = back_order[atoi(arg[1])-1];
	printf("%d---%s\n",pid, back_process[pid]);
	if(pid != 0 && back_process[pid][0]!='\0') kill(pid, atoi(arg[2]));
	else printf("enter a valid process number\n");
}

void fg(char* arg[], int n)
{
	int status;
	int pid = back_order[atoi(arg[1])-1];
	if(back_process[pid][0]=='\0' || pid ==0)
	{
		printf("no such background process exist\n");
		return;
	}
	printf("[%d] + %d running %s\n",atoi(arg[1]), pid, back_process[pid]);
	// back_process[pid][0]='\0';
	kill(pid,SIGCONT);
	flagz = 0;
	flagc = 0;
	signal(SIGTSTP, push);
	signal(SIGINT, chandler);
    while(!flagc && !flagz && pid != waitpid(pid,&status,WNOHANG) );
    if(flagc==1){
    	kill(pid,9);
    	wait(NULL);
    }
    if(flagz==1){
    	setpgid(pid,pid);
    	kill(pid,SIGSTOP);
  //   	int i;
		// strcpy(back_process[pid],arg[0]);
  // 		for (i = 0; i < back_size; ++i)
  // 		{
  // 		  if(back_process[back_order[i]][0]=='\0') 
  // 		  {
  // 		    back_order[i] = pid;
  // 		    break;
  // 		  }
  // 		}
		// if(i == back_size) back_order[back_size++] = pid;
		// printf("[%d] %d\n",i+1, pid);
    	signal(SIGCHLD, SIG_IGN);
    }
    flagc = 0;
    flagz =0;
    return;
}
void overkill()
{
	for (int i = 0; i < back_size; ++i)
	{
		int pid = back_order[i];
		if(back_process[pid][0]!='\0')
		{
			kill(pid,SIGKILL);
			wait(NULL);
			back_process[pid][0]='\0';
		}
	}
}

void bg(char* arg[], int n)
{
	int pid = back_order[atoi(arg[1])-1];
	printf("%d---%s\n",pid, back_process[pid]);
	if(pid != 0 && back_process[pid][0]!='\0') kill(pid, SIGCONT);
	else printf("enter a valid process number\n");	
}