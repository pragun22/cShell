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

// #include "headers.h"

int back_size ;
int back_order[4000];
char back_process[400000+1][300];
int j = 1;
// char * message = "remind: ";
char message[1024];
struct remind
{
  int time;
  int timer;
  char mssg[1024];
};

struct remind rem[5000];
int remind_order = 0;
void print_perms(mode_t st) 
{
    char perms[11];
    if(st && S_ISREG(st)) perms[0]='-';
    else if(st && S_ISDIR(st)) perms[0]='d';
    else if(st && S_ISFIFO(st)) perms[0]='|';
    else if(st && S_ISSOCK(st)) perms[0]='s';
    else if(st && S_ISCHR(st)) perms[0]='c';
    else if(st && S_ISBLK(st)) perms[0]='b';
    else perms[0]='l';  // S_ISLNK
    perms[1] = (st & S_IRUSR) ? 'r':'-';
    perms[2] = (st & S_IWUSR) ? 'w':'-';
    perms[3] = (st & S_IXUSR) ? 'x':'-';
    perms[4] = (st & S_IRGRP) ? 'r':'-';
    perms[5] = (st & S_IWGRP) ? 'w':'-';
    perms[6] = (st & S_IXGRP) ? 'x':'-';
    perms[7] = (st & S_IROTH) ? 'r':'-';
    perms[8] = (st & S_IWOTH) ? 'w':'-';
    perms[9] = (st & S_IXOTH) ? 'x':'-';
    perms[10] = '\0';
    printf("%s", perms);
}

void ls_l(int flag)
{
    DIR * dir; struct dirent * file; struct stat sbuf;
    char buf[128];
    struct passwd pwent, * pwentp;
    struct group grp, * grpt;
    char datestring[256];
    struct tm time;
    dir = opendir(".");
    while(file=readdir(dir)) 
    {
        char* filename = file->d_name;
        if(flag==0 && filename[0] == '.')  continue;
        stat(filename, &sbuf);
        print_perms(sbuf.st_mode);
        printf(" %d", (int)sbuf.st_nlink);
        if (!getpwuid_r(sbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
            printf(" %s", pwent.pw_name);
        else
            printf(" %d", sbuf.st_uid);
 
        if (!getgrgid_r (sbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
            printf(" %s", grp.gr_name);
        else
            printf(" %d", sbuf.st_gid);
        printf(" %7d", (int)sbuf.st_size);
        
        localtime_r(&sbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%b %e %H:%M", &time);
 
        printf(" %s %s\n", datestring, file->d_name);
    }
}

void ls(int flag)
{ 
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    dir = opendir(".");
    while ((dp=readdir(dir)) != NULL) 
    {
        // printf("%s\n",dp->d_name );
        file_name = dp->d_name; // use it
        // if ( !strcmp(file_name, ".") || !strcmp(file_name, "..") )
        if(file_name[0] == '.')
        {
            if(flag) printf("%s ",dp->d_name);
            else continue;
        } 

        else 
        {
            printf("%s ",file_name);
        }
    }
    printf("\n");
    closedir(dir);
}
void clrspaces(char *temp,int n)
{
  int whitespace = 0;
  for (int i = 0; i < n; ++i)
  {
    if(temp[i]==' ' || temp[i] == '\t') whitespace++;
    else break;
  }
  for (int i = 0; i < n; ++i)
  {
    if(i+whitespace<n) temp[i] = temp[i+whitespace];
    else temp[i]='\0';
  }
}
void pwd()
{
  char cwd[1024];
  getcwd(cwd,1024);
  printf("%s\n", cwd);  
}

void cd(char* arg[],char base[])
{
    if(arg[1]==NULL || !strcmp(arg[1],"~"))
    {
      char dir[1024];
      strcpy(dir,base); 
      if (chdir(dir)==-1)
      { 
       printf("error:: directory does not exist\n");
      }
    }
    else
    { 
      char*  dir = arg[1];
      if (chdir(dir)==-1)
      { 
       printf("error:: directory does not exist\n");
      }    
    }
    
}
void echo(char* arg[])
{
  int count = 0;
  int j = 1;
  // char *next = strtok(NULL," ");
  while(arg[j]!=NULL)
  {
  	char *next = arg[j];
  for (int i = 0; i < strlen(next); ++i)
    {
      if(next[i]=='\"') count++;
      else printf("%c",next[i]);
    } 
    // printf(" ");
   next = strtok(NULL," \t");
   j++;
  } 
  printf("\n");

}
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
      printf("exec failed\n");
      return 0;
    }
  }
  else
  {
    // printf("%d\n", waitpid(pid, &status, 0));
    // while(waitpid(pid, &status, 0))
    if(background)
    {	
    	strcpy(back_process[pid],arg[0]);
      // back_process[pid] = arg[0];
    	back_order[back_size++] = pid;
      printf("[%d] %d\n",back_size, pid);
    }

    else while( pid != wait(&status) );
      // printf("%d\n", wait(&status));

  }
}
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
    // pid = wait(NULL);
    if(waitpid(pid, &status,WNOHANG) )
    {
      printf("\n[%d] %s [%d]\n ",back_size, back_process[pid], pid);
      back_process[pid][0] = '\0';
      back_size--;
      // printf("[%d] %s [%d]\n ",j++, itao(pid), back_order[i]);
      // back_process[
    }  
  }
}
int pinfo(char* arg[], int n)
{
  int length = 0,c=0;
  size_t len = 0;
  pid_t pid;
  pid = getpid();
  if(n>1)
    pid = atoi(arg[1]);
  char *status = NULL;
  char *virmem = NULL;
  char *memory = NULL;
  char exec_path[1024] = "";
  int i;
  char path[1024];
  char path1[200];
  char path2[200];
  sprintf(path,"/proc/%d/stat", pid);
  sprintf(path1,"/proc/%d/exe", pid);
  sprintf(path2,"/proc/%d/statm", pid);
  FILE* f = fopen(path,"r");
  FILE* f2 = fopen(path2,"r");
  getline(&status,&len,f);
  getline(&virmem,&len,f2);     
  int bytes;
  bytes = readlink(path1, exec_path,200);
  exec_path[bytes] = '\0';
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
  char mem[200];
  for (int i = 0; i < 50; ++i)
  {

    if(virmem[i]==' ') break;
    mem[i] = virmem[i];
  }
  mem[i+1] = '\0';
  char st = status[7+length];
  printf("pid -- %d\n", pid);
  printf("process status -- %c memory\n", st);
  printf("%s {virtual memory}\n", mem);
  printf("Executable path -- %s\n", exec_path);
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
void relative_path(char  base[], char pwd[])
{
  char relative[1024];
  int i;
  for (i = 0; i < 1024; ++i)
  {
    relative[i] = '\0';
  }
  int n = strlen(base);
  int m = strlen(pwd);
  if(!strncmp(base,pwd,n))
  {
    relative[0] = '~';
    for (i = n; i < m; ++i)
    {
      relative[i-n+1] = pwd[i];
    }
    relative[i] = '\0';
  }
  else
  {
    for (int i = 0; i <= m; ++i)
    {
      relative[i] = pwd[i];
    }
  }
  printf("\033[1;34m%s \033[0m", relative);
}
void detect_alarm(int sig)
{
  signal(SIGALRM, SIG_IGN);    
  printf("\n%s\n", message);
  signal(SIGALRM, detect_alarm); 
}
void clck(int t,int d)
{
  int tame = 0;
  while(1)
  {
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
    if(tame/t>1) sleep(t);
    else sleep(d-tame);
    tame+=t;
  }
}
int shell_loop()
{
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
  while(1)
  {
      time_t then = time(0);
      signal(SIGALRM, detect_alarm);
      // signal(SIGCHLD, handler);
      signal(SIGCHLD, background_check);
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
        // printf("sfds\n");
        // strcpy(semsep[q],line);
        semsep[q] = line;
        // printf("fasf\n");
        // printf("%s-er\n", semsep[q]);
        line = strtok(NULL,";");
        q++;

      }
      semsep[q] = NULL;
      q = 0;
      while(semsep[q]!=NULL)
      {
        clrspaces(semsep[q],1024);
        if(semsep[q][0]=='\0') 
        {
          q++;
          continue;
        }
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
        if(!strcmp(arg[0],"exit")) exit(0);
        else if(!strcmp(arg[0],"pwd")) pwd();
        else if(!strcmp(arg[0],"cd")) cd(arg,base);
        else if(!strcmp(arg[0],"echo")) echo(arg); 
        else if(!strcmp(arg[0],"pinfo")) pinfo(arg,i);
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

            int flag1=0,flag2=0;
            int j = 0;
            while(arg[j]!= NULL)
            {
              if(!strcmp(arg[j],"-al")) 
              {
                flag1 = 1;
                flag2 = 1;
              }
              if(!strcmp(arg[j],"-la")) 
              {
                flag1 = 1;
                flag2 = 1;
              }            
              if(!strcmp(arg[j],"-a")) flag1 = 1;
              if(!strcmp(arg[j],"-l")) flag2 = 1;
              j++;
            }
            if(flag2) ls_l(flag1);
            else ls(flag1);
        }
        else exec(arg,i);
        // printf("%s\n", semsep [q]);
        q++;
      }
  }
  return 0;
}

int main(int argc, char** argv)
{
  shell_loop();
  return 0;
} 