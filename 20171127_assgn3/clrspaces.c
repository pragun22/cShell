#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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