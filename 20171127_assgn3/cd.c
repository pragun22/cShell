#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>

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