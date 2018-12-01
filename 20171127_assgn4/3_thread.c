#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
int arr[1000000];
// int *arr;
typedef struct Input{
	int low;
	int high;
} Input;
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
void* mergesort(void* args)
{
	Input * in = (Input*)args;
	int l = in->low;
	int r = in->high;
	int mid = l +(r-l)/2;
	if(in->high-in->low<5)
	{
		selectionsort(in->low,in->high);
		return 0;

	}
	pthread_t t1,t2;
	Input* arg1=(Input*)malloc(sizeof(Input*));
	arg1->low=l;
	arg1->high=mid;
	Input* arg2=(Input*)malloc(sizeof(Input*));
	arg2->low=mid+1;
	arg2->high=r;
	if(l>=r) return 0;
	pthread_create(&t1,NULL,mergesort,arg1);
	pthread_create(&t2,NULL,mergesort,arg2);
	// printf("check1\n");
	pthread_join(t1,0);
	pthread_join(t2,0);
	merge(l,mid,r);
}

int main()
{
	int n,i;
	scanf("%d",&n);
	for (i = 0; i < n; ++i)
	{
		scanf("%d",&arr[i]);
	}
		clock_t start = clock();

	pthread_t t;
	Input * input = (Input*)malloc(sizeof(Input));
	input->low =0;
	input->high=n-1;
	pthread_create(&t,NULL,mergesort,input);
	pthread_join(t,0);
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