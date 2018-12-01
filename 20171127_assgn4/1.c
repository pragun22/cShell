#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h> 
//mutexes
pthread_mutex_t Orgmutex;
pthread_mutex_t plmutex;
pthread_mutex_t rfmutex;
pthread_mutex_t stmutex;
pthread_cond_t start;
pthread_cond_t st;
pthread_cond_t cond_game;
pthread_cond_t cond;
sem_t plsem;
typedef struct Player{
	pthread_mutex_t pl;
	pthread_cond_t condition;
	int type;
	int id;
	pthread_t pid;

} Player;
typedef struct Ref{
	pthread_mutex_t rf;
	pthread_cond_t condition;
	int type;
	int id;
	pthread_t pid;
} Ref ;
//functions
void* enterAcademy(void* args);
void meetOrganiser(void* args,int type);
void enterCourt(void* args,int type);
void warmUp(Player* p);
void adjustEquipment(Ref* ref);
void startGame(int id);
void starwars(void* args);
Player* player_init(Player * player,int id,int type);
Ref* ref_init(Ref* ref,int id , int type);
int playerc;
int refc;
int playa[2];
int condition_play=0;
int condition;
int warm = 0;
int start_game = 0;


int main()
{
	pthread_mutex_init(&Orgmutex, NULL);
	pthread_mutex_init(&plmutex, NULL);
	pthread_mutex_init(&rfmutex, NULL);
	pthread_mutex_init(&stmutex, NULL);
	pthread_cond_init(&cond,NULL);
	pthread_cond_init(&cond_game,NULL);
	pthread_cond_init(&st,NULL);
    sem_init(&plsem,0,2);
	int n,i;
	scanf("%d",&n);
    // n+=1;
	int plcount=0;
	int refcount=0;
	int m = 2*n;
	Player** player=(Player**)malloc(sizeof(Player*)*(m));
	Ref** ref=(Ref**)malloc(sizeof(Ref*)*(n));
	for(i =1;i<=3*n+1;i++)
	{
		sleep(rand()%3);
		int p = 2*n - plcount;
		int q = n-refcount;
		if(p-q>0)
		{
			plcount++;
			player[i] = player_init(player[i],plcount,1);
			pthread_create(&(player[i]->pid),NULL,enterAcademy,player[i]);
		}
		else
		{
			if(refcount<=n) refcount++;
			ref[i] = ref_init(ref[i],refcount,0);
			pthread_create(&(ref[i]->pid),NULL,enterAcademy,ref[i]);
		}

	}
	return 0;
}
Player* player_init(Player* player,int id , int type)
{
	player = (Player*)malloc(sizeof(Player));
	player->id = id;
	player->type = type;
	pthread_mutex_init(&(player->pl),NULL);
	pthread_cond_init(&(player->condition),NULL);
	return player;
}
Ref* ref_init(Ref* ref,int id , int type)
{
	ref = (Ref*)malloc(sizeof(Player));
	ref->id = id;
	ref->type = type;
	pthread_mutex_init(&(ref->rf),NULL);
	pthread_cond_init(&(ref->condition),NULL);

	return ref;
}

void* enterAcademy(void* args)
{
	Player* player = (Player*)args;
	if(player->type==1)
	{
		printf("player %d entering the academy\n",player->id);
	}
	else
	{
		printf("referee %d entering the academy\n",player->id);
	}
	// sleep(1);
	meetOrganiser(player,player->type);
}
void enterCourt(void* args,int type)
{
	// printf("%ld\n",pthread_self());
	if(type==1)
	{
		Player* p = (Player*)args;
        while(playerc<2 || refc==0);
        printf("player %d entered court\n",p->id);
        warmUp(p);
	}
	else
	{
		Ref* ref = (Ref*)args;
        while(playerc<2 || refc==0);
		printf("referee %d entering court\n",ref->id);
        start_game = 0;
		adjustEquipment(ref);
	}	
}
void warmUp(Player* p)
{
	pthread_mutex_lock(&Orgmutex);
	printf("player %d warming up\n",p->id);
	warm++;
	pthread_mutex_unlock(&Orgmutex);
	if(warm==2) pthread_cond_signal(&start);
	

}
void adjustEquipment(Ref* ref)
{
	while(warm<2)
	{
		pthread_cond_wait(&start,&Orgmutex);
	}
	printf("Referee %d adjusting equipments\n",ref->id);
	startGame(ref->id);
}
void startGame(int id)
{
	printf("\033[32;1;4mGame started by Referee %d\033[0m\n",id);
	warm=0;
	start_game=1;
	pthread_cond_signal(&st);

}
void meetOrganiser(void * args,int type)
{
    if(type==1)
    {
		Player* player = (Player*)args;
		sem_wait(&plsem);
		printf("player %d meeting organiser\n",player->id);
		playerc++;
		playa[playerc-1]=player->id;
        enterCourt(player,1);
    	// {
	    // 	pthread_cond_wait(&st,&stmutex);
    	// }
        playerc--;
        while(!start_game || playerc==1);
		sem_post(&plsem);


    }
    else if(type==0)
    {
        Ref* ref = (Ref*)args;
		pthread_mutex_lock(&rfmutex);
		printf("referee %d meeting organiser\n",ref->id);
		refc=ref->id;
        enterCourt(ref,0);
        while(!start_game);
    	// {
		//     pthread_cond_wait(&st,&stmutex);
	    // }
        refc=0;
		pthread_mutex_unlock(&rfmutex);
    }
}
