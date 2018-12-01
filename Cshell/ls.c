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