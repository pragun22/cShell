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

int pinfo(char* arg[],char base[])
{
  int length = 0,c=0;
  size_t len = 0;
  pid_t pid;
  if(arg[1]!= NULL)
    pid = atoi(arg[1]);
  else pid = getpid();
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
  if(!f || !f2)
  {
    printf("could not open process\n");
    return 0;
  }
  getline(&status,&len,f);
  getline(&virmem,&len,f2);
  int bytes;
  bytes = readlink(path1, exec_path,200);
  exec_path[bytes] = '\0';

  char relative[1024];
  for (i = 0; i < 1024; ++i)
  {
    relative[i] = '\0';
  }
  int n = strlen(base);
  int m = strlen(exec_path);
  if(!strncmp(base,exec_path,n))
  {
    relative[0] = '~';
    for (i = n; i < m; ++i)
    {
      relative[i-n+1] = exec_path[i];
    }
    relative[i] = '\0';
  }
  else
  {
    for (i = 0; i <= m; ++i)
    {
      relative[i] = exec_path[i];
    }
  }

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
  for (i = 0; i < 50; ++i)
  {

    if(virmem[i]==' ') break;
    mem[i] = virmem[i];
  }
  mem[i+1] = '\0';
  int temp = pid;
  int digits = 0;
  while(temp>0)
  {
    temp/=10;
    digits++;
  }
  char st = status[digits+length+3];
  printf("pid -- %d\n", pid);
  printf("process status -- %c memory\n", st);
  printf("%s {virtual memory}\n", mem);
  printf("Executable path -- %s\n", relative);
}
