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
#include "headers.h"
int shell_loop()
{
	pid_t ppid = -1;
  char * usr = (char * )malloc(20*sizeof(char));
  char hostname[1024];
  usr = getlogin();
  gethostname(hostname,1024);
  char base[1024];
  getcwd(base,1024);
  // time_t now =  time(0);
  int j;
  for (j = 0; j < 400001; ++j)
  {
    back_process[j][0] = '\0';
  }
  char* semsep[1024];
  signal(SIGQUIT,SIG_IGN);
  signal(SIGTSTP,SIG_IGN);
  signal(SIGINT,SIG_IGN);
  while(1)
  {
      in = 0;
      out = 0;
      time_t then = time(0);
      signal(SIGALRM, detect_alarm);
      signal(SIGCHLD, background_check);
      signal(SIGINT,chandler);  //clrt+c
      signal(SIGTSTP,push);  // cltr + z
      char cwd[1024];
      getcwd(cwd,1024);
      // printf("%s-%d\t%s-%d\n",base, n,cwd, m);
      char cmd_line[1024];
      char *relative;
      int i;
      printf("\033[1;33m> %s@%s: \033[0m", usr, hostname);
     relative_path(base, cwd);
      if(!scanf("%[^\n]%*c", cmd_line))
      {
        getchar();
        continue;
      }
      char * line = strtok(cmd_line,";");
      int q =0;
      while(line != NULL)
      {
        semsep[q] = line;
        line = strtok(NULL,";");
        q++;
      }
      semsep[q] = NULL;
      q = 0;
      while(semsep[q]!=NULL)
      {
      ppid = -1;
        clrspaces(semsep[q],1024);
        if(semsep[q][0]=='\0')
        {
          q++;
          continue;
        }
        if(checkpipes(semsep,q))
        {
          execute_with_pipes(semsep,q);
          q++;
          // execute_with_pipes(semsep);
          continue;
        }
        else
        {
          char * cmd = strtok(semsep[q], " \t");
          i = 0;
          char* arg[1024];
          while(cmd!=NULL)
          {
            arg[i] = cmd;
            cmd = strtok(NULL, " \t");
            i++;
          }
          arg[i] = NULL;
          int number_arguments = i;
          check_redirection(arg);
          if(!strcmp(arg[0],"exit") || !strcmp(arg[0],"quit")) exit(0);
          if(in || out)
          {
            redirection(arg,number_arguments);
          }
          else if(!strcmp(arg[0],"fg"))
          {
            if(number_arguments!=2)
            {
              printf("wrong syntax\nuse - fg <process_number>");
            }
            else fg(arg,number_arguments);
          }
		     else if(!strcmp(arg[0],"bg"))
          {
            if(number_arguments!=2)
            {
              printf("wrong syntax\nuse - bg <process_number>");
            }
            else bg(arg,number_arguments);
          }
          else  if(!strcmp(arg[0],"setenv"))
          {
          	if(number_arguments!=2 && number_arguments !=3)
          	{
          		printf("wrong syntax\nuse syntax - setenv env_name [value(can be none)]\n");
          	}
            else{
          	if(arg[2]!=NULL) setenv(arg[1], arg[2], 1);
          	else setenv(arg[1], "", 1);
            }
          }
          else if(!strcmp(arg[0], "unsetenv"))
          {
          	if(number_arguments!=2)
          	{
          		printf("**error--wrong syntax\nuse syntax - unsetenv env_name\n");
          	}
            else unsetenv(arg[1]);
          }
          else if(!strcmp(arg[0], "jobs"))
          {
          	if(number_arguments!=1)
          	{
          		printf("no arguments needed just write jobs\n");
          	}
          	else jobs();
          }
          else if (!strcmp(arg[0], "kjob"))
          {
          	if(number_arguments!=3)
          	{
          		printf("invalid arguments\nsyntax: kjobs <jobnumber> <signalnumber>\n");
          	}
          	else kjobs(arg,number_arguments);

          }
          else if(!strcmp(arg[0],"overkill"))
          {
            if(number_arguments!=1)
            {
              printf("wrong syntax.\nuse only overkill");
            }
            else overkill();
          }
          else if(!strcmp(arg[0],"pwd"))
          {
            if(arg[1]!=NULL)
            {
              printf("no flags required\n");
            }
            else pwd();
          }
          else if(!strcmp(arg[0],"cd"))
          {
            if(arg[2]!=NULL)
            {
              printf("**error** too many arguments\n");
            }
            else cd(arg,base);
          }
          else if(!strcmp(arg[0],"echo")) echo(arg);
          else if(!strcmp(arg[0],"pinfo")) pinfo(arg,base);
          else if(!strcmp(arg[0],"clock"))
          {
            if(arg[1]==NULL)
            {
              printf("enter flags\n clock -t <interval> -n <duration>\n");
            }
            else if(strcmp(arg[1],"-t"))
            {
              printf("enter a valid argument\n");
            }
            else if(arg[2]==NULL)
            {
              printf("enter a time\n");
            }
            else if(arg[3]==NULL && strcmp(arg[3],"-n"))
            {
              printf("enter -n flag for duration of command\n clock -t <interval> -n <duration>\n");
            }
            else if(arg[4] == NULL)
            {
              printf("enter duration time\n");
            }
            else clck(atoi(arg[2]),atoi(arg[4]));
          }
          else if(!strcmp(arg[0],"remindme"))
          {
            remindme(arg);
            alarm(atoi(arg[1]));
          }
          else if(!strcmp(arg[0],"ls"))
          {

              int flag1=0,flag2=0,flag3=0;
              int j = 1;
              while(arg[j]!= NULL)
              {
                if(!strcmp(arg[j],"-al"))
                {
                  flag1 = 1;
                  flag2 = 1;
                }
                else if(!strcmp(arg[j],"-la"))
                {
                  flag1 = 1;
                  flag2 = 1;
                }
                else if(!strcmp(arg[j],"-a")) flag1 = 1;
                else if(!strcmp(arg[j],"-l")) flag2 = 1;
                else
                {
                	flag3 = 1;
                	break;
                }
                j++;
              }
              if(flag3)
              {
                	printf("enter proper flags such as -[al]\n");
              }
              else if(flag2 && flag3==0) ls_l(flag1);
              else if(flag3==0)  ls(flag1);
          }
          else exec(arg,number_arguments);
          q++;
        }
      }
  }
  return 0;
}

int main(int argc, char** argv)
{
  shell_loop();
  return 0;
}
