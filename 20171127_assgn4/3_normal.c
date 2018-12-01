#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>	
#include <sys/ipc.h>
#include <time.h>
void merge(int ar1[],int start,int mid,int end)
{
	int  sz1=mid-start +1;
	int sz2=end -mid;
	int i,j,k;//variables for loop
	int nar1[sz1];//temp array 1
	int nar2[sz2];//temp array 2
	for(i=0;i<sz1;i++)
	{
		nar1[i]=ar1[start +i];
	}
	for(i=0;i<sz2;i++)
	{
		nar2[i]=ar1[mid+1+i];

	}
	i=0;j=0;k=start;
	while(i<sz1 && j<sz2)
	{

		if(nar1[i]>nar2[j])
		{
			ar1[k]=nar2[j];
			j++;
			k++;
		}
		if(nar1[i]<=nar2[j])
		{
			ar1[k]=nar1[i];
			i++;
			k++;
		}
	}
	while(i<sz1)
	{
		ar1[k]=nar1[i];
		i++;
		k++;
	}
	while(j<sz2)
	{
		ar1[k]=nar2[j];
		j++;
		k++;
	}

}
int mergesort(int ar1[],int start,int end)
{
	if(start<end)
	{
		int mid=(start+(end-start)/2);
		mergesort(ar1,start,mid);
		mergesort(ar1,mid+1,end);
		merge(ar1,start,mid,end);
	}
	return 0;
}
int main()
{
	int n,j;
	scanf("%d",&n);
	int arr[n];
	for(j=0;j<n;j++)
	{
		scanf("%d",&arr[j]);
	}
	clock_t start = clock();
	mergesort(arr,0,n-1);
	clock_t end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	for(j=0;j<n;j++)
	{
		printf("%d ",arr[j]);
	}
	printf("\ntime taken %f\n",cpu_time_used);

	return 0;
}