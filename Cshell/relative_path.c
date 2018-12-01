#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
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