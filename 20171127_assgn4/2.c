#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

//global variables
pthread_mutex_t mutex_booth;
pthread_cond_t cond_booth;
typedef struct Evm Evm;
typedef struct Voter Voter;
typedef struct Booth Booth;
typedef struct Booth{
    int id;
    pthread_cond_t cond_evm;
    pthread_mutex_t mutex_evm;
    pthread_t pid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int inprogress;
    Evm** evm;
    Voter** voter;
    Evm* free_evm;
    Evm* need_evm;
    int nevms;
    int nvoter;
    int done_voters;
    int test;
}Booth;
typedef struct Evm{
    int id;
    pthread_t pid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Booth * booth;
    int slots;
    int occupied;
    int free;
}Evm;
typedef struct Voter{
    int id;
    pthread_t pid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Booth* booth;
    Evm* evm;
}Voter;
Booth * booth_init(Booth* b,int id,int nevms,int voter);
Evm * evm_init();
Voter * voter_init();
void * booth_fn(void* args); 
void * evm_fn(void* args); 
void * voter_fn(void* args); 
void polling_evm_ready(Booth* b, int count);
void voter_wait_for_evm(Voter* v,Booth* b);
void voter_in_slot(Booth* b,Voter* v);
int main()
{
    pthread_mutex_init(&mutex_booth,NULL);
    pthread_cond_init(&cond_booth,NULL);
    int nbooths,i;
    printf("Enter no. of booths  ");
    scanf("%d",&nbooths);
    int evms[nbooths],voters[nbooths];
    Booth** booth = (Booth**)malloc(sizeof(Booth*)*(nbooths));
    for(i = 0;i<nbooths;i++)
    {
        printf("Number : evms  voters for booth-%d\n",i+1);
        scanf("%d %d",&evms[i],&voters[i]);
    }
    printf("Now starting election process\n");
    for(i = 0; i<nbooths;i++)
    {
        booth[i]= booth_init(booth[i],i+1,evms[i],voters[i]);
    }
    for(i = 0; i<nbooths;i++)
    {
        pthread_create(&(booth[i]->pid),NULL,booth_fn,booth[i]);
    }
    for(i = 0; i<nbooths;i++)
    {
        pthread_join(booth[i]->pid,0);
    }    
    printf("\033[34;1;4mElection at all booths completed sucessfully\033[0m\n");
    exit(0);
    return 0;
}
Booth * booth_init(Booth* b,int id,int nevms,int voter)
{
    b = (Booth*)malloc(sizeof(Booth));
    b->id = id;
    b->done_voters = 0;
    b->inprogress = 0;
    b->test = 0;
    b->nevms = nevms;
    b->nvoter= voter;
    b->evm = (Evm**)malloc(sizeof(Evm*)*nevms);
    b->voter = (Voter**)malloc(sizeof(Voter*)*voter);
    b->free_evm = NULL;
    b->need_evm = NULL;
    pthread_mutex_init(&(b->mutex_evm),NULL);
    pthread_mutex_init(&(b->mutex), NULL);
    pthread_cond_init(&(b->cond), NULL);
    pthread_cond_init(&(b->cond_evm),NULL);
    return b;
}
Evm* evm_init(Evm* evm,int id, Booth* booth)
{
    evm = (Evm*)malloc(sizeof(Evm));
    pthread_cond_init(&evm->cond,NULL);
    pthread_mutex_init(&evm->mutex,NULL);
    evm->id = id;
    evm->slots = (rand()+1)%10;
    evm->occupied = 0;
    evm->booth = booth;
    evm->free= 1;
    return evm;
}
Voter* voter_init(Voter* voter,int id,Booth* booth)
{
    voter = (Voter*)malloc(sizeof(Voter));
    voter->id = id;
    voter->evm = NULL;
    voter->booth = booth;
    return voter;
}
void * booth_fn(void* args)
{
    int i;
    Booth * b = (Booth*)args;
    printf("Initializing Booth-%d\n",b->id);
    for(i=0;i<b->nevms;i++)
    {
        sleep(1);
        b->evm[i] = evm_init(b->evm[i],i+1,b);
        pthread_create(&(b->evm[i]->pid),NULL,evm_fn,b->evm[i]);
    }
    for(i=0;i<b->nvoter;i++)
    {
        sleep(1);
        b->voter[i] = voter_init(b->voter[i],i+1,b);
        pthread_create(&(b->voter[i]->pid),NULL,voter_fn,b->voter[i]);
    }
    while(b->done_voters!=b->nvoter)
    {
        pthread_cond_wait(&cond_booth,&mutex_booth);
    }
    // for(i=0;i<b->nevms;i++)
    // {
    //     pthread_join(b->evm[i]->pid,0);
    // }
    // for(i=0;i<b->nvoter;i++)
    // {
    //     pthread_join(b->voter[i]->pid,0);
    // }
    printf("\033[31;1;4mVoting at Booth %d completed \033[0m\n",b->id);
}
void * evm_fn(void * args)
{
        Evm* e = (Evm*)args;
    while(e->booth->done_voters!=e->booth->nvoter)
    {
        printf("evm %d activated of booth %d\n",e->id,e->booth->id);
        while(e->booth->inprogress!=0)
        {
            pthread_cond_wait(&(e->booth->cond_evm),&(e->booth->mutex_evm));
        }
        pthread_mutex_lock(&(e->booth->mutex_evm));
        e->booth->inprogress = e->id;
        while(e->slots-e->occupied==0);
        e->free = 1;
        polling_evm_ready(e->booth,e->slots-e->occupied);
        e->free = 0;
        sleep(0.5);
        pthread_mutex_unlock(&(e->booth->mutex_evm));
    }

}
void * voter_fn(void * args)
{
    Voter* v = (Voter*)args;
    printf("Voter %d entered of booth %d\n",v->id,v->booth->id);
    // printf("this iisss--%d",v->booth->id);
    while(v->booth==NULL);
    voter_wait_for_evm(v,v->booth);
    while(v->booth->free_evm==NULL);
    voter_in_slot(v->booth,v);
}
void polling_evm_ready(Booth* b, int count)
{
    int freeplaces = b->evm[b->inprogress-1]->slots - b->evm[b->inprogress-1]->occupied;
    b->test = 10000;
    printf("EVM %d of Booth %d has %d free slots to allot to a voter\n",b->inprogress,b->id, count);
    while(freeplaces!=0)
    {
        freeplaces = b->evm[b->inprogress-1]->slots - b->evm[b->inprogress-1]->occupied;
        if(b->done_voters==b->nvoter)
        {
            break;
        }
    }
    b->free_evm = b->evm[b->inprogress-1];
    b->inprogress= 0;
    // printf("you are freed\n");
    pthread_cond_broadcast(&(b->cond_evm)); 
}
void voter_wait_for_evm(Voter* v,Booth * b)
{
    printf("voter %d waiting for evm to be assigned at Booth %d\n",v->id,v->booth->id);
    while(b->inprogress<=0);
    v->evm = b->evm[b->inprogress-1];
    v->evm->occupied++;    
    printf("voter %d assigned evm %d at Booth %d\n",v->id,v->evm->id,v->booth->id);
    v->booth->done_voters++;
    if(v->evm->occupied==b->nvoter)
    {
            // pthread_cond_broadcast(&cond_booth);
    }    
    else if(v->evm->occupied==v->evm->slots)
    {
        printf("Evm %d fully occupied\n",v->evm->id);
    }
}
void voter_in_slot(Booth* b,Voter* v)
{
    while(v->evm==NULL);
    while(v->evm->free!=0);
    pthread_mutex_lock(&b->mutex);
    if(v->evm!=NULL)
    {
        printf("Vote of Voter %d registered at evm %d of booth %d\n",v->id,v->evm->id,b->id);
        v->evm->occupied--;
        if(v->evm->occupied==0)
        {
            printf("\033[33;1;4mVoting at evm %d completed of booth %d\033[0m\n",v->evm->id,v->booth->id);
            b->free_evm = NULL;
            if(b->done_voters==b->nvoter) pthread_cond_broadcast(&cond_booth);
        }
        // if(v->id+1==b->nvoter)
        // {
        // }
        // if(v->evm->occupied==0 && b->done_voters==b->nvoter )
        // {
        //     printf("\033[33;1;4mVoting at evm %d completed \033[0m\n",v->evm->id);
        // }
        
        v->evm=NULL;
    }
    pthread_mutex_unlock(&b->mutex);
}