#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>	
#include <sys/ipc.h>
#include <time.h>
int *arr;
void merge(int start,int mid, int end)
{
	int  sz1=mid-start +1;
	int sz2=end -mid;
	int i,j,k;//variables for loop
	int nar1[sz1];//temp array 1
	int nar2[sz2];//temp array 2
	for(i=0;i<sz1;i++)
	{
		nar1[i]=arr[start +i];
	}
	for(i=0;i<sz2;i++)
	{
		nar2[i]=arr[mid+1+i];

	}
	i=0;j=0;k=start;
	while(i<sz1 && j<sz2)
	{

		if(nar1[i]>nar2[j])
		{
			arr[k]=nar2[j];
			j++;
			k++;
		}
		if(nar1[i]<=nar2[j])
		{
			arr[k]=nar1[i];
			i++;
			k++;
		}
	}
	while(i<sz1)
	{
		arr[k]=nar1[i];
		i++;
		k++;
	}
	while(j<sz2)
	{
		arr[k]=nar2[j];
		j++;
		k++;
	}
}
void swap(int a,int b)
{
	int temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}
void selectionsort(int start, int end)
{
	int i,j;
	for ( i = start; i <= end; ++i)
	{
		for ( j = i+1; j <= end; ++j)
		{
			if(arr[i]>arr[j])
			{
				swap(i,j);
			}
		}
	}
}
void mergesort(int start, int end)
{
	int len = (end - start +1);
	int mid = start + (end-start)/2;
	if(len<5)
	{
		selectionsort(start,end);
		return;
	}
	pid_t lpid,rpid;
	lpid = fork();
	if(lpid<0)
	{
		printf("error in forking\n");
		exit(0);
	}
	else if(lpid==0)
	{
		mergesort(start,mid);
		exit(0);
	}
	else
	{
		rpid = fork();
		if(rpid<0)
		{
			printf("eror in forking\n");
			exit(0);
		}
		else if(rpid==0)
		{
			mergesort(mid+1,end);
			exit(0);
		}
	}
	int status;
	waitpid(lpid,&status,0);
	waitpid(rpid,&status,0);

	merge(start,mid,end);
}

int main()
{
	int n,i;
	scanf("%d",&n);
	int shmid = shmget(IPC_PRIVATE, sizeof(int)*n, IPC_CREAT|0666);
	if(shmid == -1)
	{
		perror("shget failed");
		exit(0);
	}
	arr =  shmat(shmid, 0 ,0);
	for (i = 0; i < n; ++i)
	{
		scanf("%d",&arr[i]);
	}
	clock_t start = clock();
	mergesort(0,n-1);
	clock_t end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	for ( i = 0; i < n; ++i)
	{
		// printf("%d-i\n",i);
		printf("%d ",arr[i] );
	}
	printf("\ntime taken %f\n",cpu_time_used);
	return 0;
}