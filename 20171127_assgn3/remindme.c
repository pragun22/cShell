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
extern char message[1024];
extern int in;
extern int out;
void detect_alarm(int sig)
{
  signal(SIGALRM, SIG_IGN);    
  printf("\n%s\n", message);
  signal(SIGALRM, detect_alarm); 
}

void remindme(char* arg[])
{
  int i = 2;
  message[0]='\0';
  strcpy(message, "Remind: ");
  while(arg[i]!=NULL)
  {
    strcat(message,arg[i]);
    strcat(message," ");
    i++;
  }
}
void clck(int t,int d)
{
  int tame = 0;
  int temp = d/t;
  while(1)
  {
    temp--;
    if(tame> d ) break;
    char path[2048];
    sprintf(path,"/proc/driver/rtc");
    FILE * f = fopen(path,"r");
    if(!f)
    {
      printf("could not open file\n");
      return;
    }
    char *buf[2048];
    char *buf2[2048];
    fscanf(f,"%*s %*s %s ",buf);
    fscanf(f,"%*s %*s %s ",buf2);
    printf("%s, %s\n",buf2 ,buf);
    fclose(f);
    if(temp < 0) sleep(d-tame);
    else sleep(t);
    tame+=t;
  }
}
