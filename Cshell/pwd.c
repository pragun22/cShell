#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
void pwd()
{
  char cwd[1024];
  getcwd(cwd,1024);
  printf("%s\n", cwd);  
}
