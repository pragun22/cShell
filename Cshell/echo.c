#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    printf(" ");
   next = strtok(NULL," \t");
   j++;
  } 
  printf("\n");

}