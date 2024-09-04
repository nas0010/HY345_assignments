#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#define RED 0
#define BLUE 1
#define WEST 0
#define	EAST 1


void red();
void blue();
void reset();
void color(int c);

typedef struct pedestrian{
	 int id;
	 int color;
	 int direction;
	 int position;
	 char priority;
}t_ped;

int strongColor=0,strongDirection=0,dircnt=0,colorcnt=0,ph=0,numofWalkers=0;
int count_phase1=0,count_phase2=0,count_phase3=0,count_phase4=0;
	

t_ped **road;

void phase(int n,t_ped **p,int num);
pthread_mutex_t print,count,walk,side,out;

void color(int c){
	if (c)blue();
	else red();
}
void red () {
	printf("\033[1;31m");
}
void blue () {
	printf("\033[0;34m");
}
void reset () {
	printf("\033[0m");
}

void printRoad(int num,t_ped **r){
	int j;
	for(j=0;j<2*num;j++){
		printf("||");
		if(j==(num))	printf("\n||");
		if(r[j]->id>=0){
			color(r[j]->color);
			if(r[j]->direction)	printf(" %d> ", r[j]->id);
			else printf(" <%d ",r[j]->id);
			reset();
		}else printf("    ");
       }	
	printf("||\n\n");
}
void getPriorities(t_ped **road){
	int i=0;
	for(i=0;i<numofWalkers;i++){
		if(road[i]->color==strongColor && road[i]->direction==strongDirection){
			road[i]->priority='h';       
			count_phase1++;
		}else if(road[i]->color==strongColor && road[i]->direction!=strongDirection){
			road[i]->priority='m';       
			count_phase2++;
		}else if(road[i]->color!=strongColor && road[i]->direction==strongDirection){
			road[i]->priority='l';       
			count_phase3++;
		}else{
		       	count_phase4++;
			road[i]->priority='d';
		}
	}

}

int getCounter(int id){
	switch (id){
	case 1:
		return count_phase1;
		break;
	case 2:
		return count_phase2;
		break;
	case 3:
		return count_phase3;
		break;
	case 4:
		return count_phase4;
		break;
	default:
		return -44;
	}
}

void lowerCounter(t_ped *ped){
	int tmp1=count_phase1,tmp2=count_phase2,tmp3=count_phase3,tmp4=count_phase4;
	if(ped->priority=='h') --count_phase1;
	else if(ped->priority=='m')--count_phase2;
	else if(ped->priority=='l') --count_phase3;
	else if(ped->priority=='d') --count_phase4;
	
	if(tmp1>0 && count_phase1==0){
		count_phase1=892;
		phase(numofWalkers,road,2);
		ph=2;
	}else if(tmp2>0 && count_phase2==0 ){
		count_phase2=892;
		ph=3;
		phase(numofWalkers,road,3);
	}else if( tmp3>0 && count_phase3==0){
		count_phase3=892;
		ph=4;
		phase(numofWalkers,road,4);
	}else if( tmp4>0 && count_phase4==0){
		printf("amhn");
		ph=5;
		count_phase4=892;
	}
}
void initialiseandPrint(int num,t_ped **road,t_ped **newdromos){
	int i=0,j=0,cnt1=0,cnt2=0;
	t_ped *temp[num-dircnt], *t;
	t=malloc(sizeof(t_ped));
	t->id=-1;
	t->direction=-1;
	t->color=-1;
	t->position=-1;
	t->priority='n';
	//east direction
    	for(j=0;j<num;j++){
		if(road[j]->direction){
			newdromos[cnt1]=road[j];
			newdromos[cnt1]->position=cnt1;
			cnt1++;
		}else {
			temp[cnt2]=road[j];
			cnt2++;
			}
	}
	//west direction
	for(i=0;i<(num-dircnt);i++){
		newdromos[cnt1+i]=temp[i];
		newdromos[cnt1+i]->position=cnt1+i;
			
	} 
	//empty sidewalk
	for(j=num;j<2*num;j++){
		newdromos[j]=t;
	}
	
	printRoad(num,newdromos);

}
void phase(int num,t_ped **road,int id){
	int i;
	t_ped *t;
	t=malloc(sizeof(t_ped));
	t->id=-1;
	t->direction=-1;
	t->color=-1;
	t->position=-1;
	t->priority='n';
	switch(id){
		case 1:
		for(i=0;i<num;i++){
			if(road[i]->color!=strongColor || road[i]->direction!=strongDirection){
				road[i+num]=road[i];
				road[i+num]->position=i+num;
				road[i]=t;
			}	
		}
		printf("First phase:\n");
		break;
			
		case 2:
		for(i=num;i<2*num;i++){
			if(road[i]->color==strongColor && road[i]->id!=-1){
				road[i-num]=road[i];
				road[i-num]->position=i-num;
				road[i]=t;
			}	
		}
		printf("Second phase:\n");
		break;

		case 3:
		for(i=num;i<num*2;i++){
			if(road[i]->direction==strongDirection && road[i]->id!=-1){
				road[i-num]=road[i];
				road[i-num]->position=i-num;
				road[i]=t;
			}	
		}
		printf("Third phase:\n");
		break;

		default:
		for(i=num;i<num*2;i++){
			if(road[i]->id!=-1){
				road[i-num]=road[i];
				road[i-num]->position=i-num;
				road[i]=t;
			}	
		}
		printf("Final phase:\n");
		break;	
	}
	printRoad(num,road);

}

int checkifEmpty(t_ped**road){
	int i,flag;
	printf("tsekarw adeiosunu\n");
	printRoad(numofWalkers,road);
	for(i=0;i<numofWalkers;i++){
		if(road[i]->id==-1) flag++;
	}
	if(flag==numofWalkers) return 1;
	else return 0;
}
	

void *route(void *args){
	int i,j,flag=0;
	t_ped* secondary=malloc(sizeof(t_ped*));
	int ped =*((int*)args);
	char tmp;
	t_ped *t;
	t=malloc(sizeof(t_ped));
	t->id=-1;
	t->direction=-1;
	t->color=-1;
	t->position=-1;
	t->priority='n';
	for(ph=1;ph<5;){
	if(road[ped]->id!=-1){
		if(road[ped]->direction==EAST){
			for(i=ped;i<numofWalkers-1;){	
				if(road[i+1]->id==-1){
					pthread_mutex_lock(&walk);
					road[i+1]=road[i];
					road[i+1]->position=i+1;
					road[i]=t;
					pthread_mutex_lock(&print);
					printRoad(numofWalkers,road);
					pthread_mutex_unlock(&print);
					i++;
					if(i==numofWalkers-1) {
						secondary=road[i];
						road[i]=t;
						pthread_mutex_lock(&print);
						printRoad(numofWalkers,road);
						pthread_mutex_lock(&print);
						lowerCounter(secondary);
					}
					pthread_mutex_unlock(&walk);
				}
			}
		}else if(road[ped]->direction==WEST){
			for(i=ped;i>0;){	
				if(road[i-1]->id==-1){
					pthread_mutex_lock(&walk);
					road[i-1]=road[i];
					road[i-1]->position=i-1;
					road[i]=t;
					pthread_mutex_lock(&print);
					printRoad(numofWalkers,road);
					pthread_mutex_unlock(&print);
					i--;
					if(i==0) {
						secondary=road[i];
						road[i]=t;
						pthread_mutex_lock(&print);
						printRoad(numofWalkers,road);
						pthread_mutex_unlock(&print);
						lowerCounter(secondary);
					}
					pthread_mutex_unlock(&walk);
				}
			}
		}
	}
	}
//	pthread_mutex_lock(&print);
//	if(checkifEmpty(road)==1){
//			ph++;
//			phase(numofWalkers,road,ph);
//		}
//	pthread_mutex_unlock(&print);
	free(args);
	return NULL;
}

int main(int arg, char **argv){
	int i,id,color,direction;	
	t_ped *pedestrian;
  	if(strcmp(argv[1], "-p"))  exit(1);

 	numofWalkers=atoi(argv[2]);
	pthread_t road_t[numofWalkers];
	t_ped *dromos[numofWalkers];
	t_ped *firstdromos[2*numofWalkers];
	road=firstdromos;
	srand(time(NULL));

	for(id=0;id<numofWalkers;id++){
		pedestrian=malloc(sizeof(t_ped));
		color = rand()%2;
		direction=rand()%2;
		pedestrian->color=color;
		pedestrian->id=id;
		pedestrian->direction=direction;
		dromos[id]=pedestrian;  
		if(color==RED)colorcnt++;
		if(direction==EAST)dircnt++;
	}


	if(colorcnt>(numofWalkers/2)) strongColor=RED; 
        else strongColor=BLUE;
	if(dircnt>(numofWalkers/2)) strongDirection=EAST; 
	else strongDirection=WEST;

	initialiseandPrint(numofWalkers,dromos,firstdromos);
	getPriorities(firstdromos);
	ph=1;
	phase(numofWalkers,firstdromos,ph);
	
	pthread_mutex_init(&print,NULL);
	pthread_mutex_init(&count,NULL);
	pthread_mutex_init(&walk,NULL);
	pthread_mutex_init(&side,NULL);
	pthread_mutex_init(&out,NULL);
	
	
	//create threads
	for(i=0;i<numofWalkers;i++){
		int *tmp=malloc(sizeof(*tmp));
		*tmp=i;
		if (pthread_create(&road_t[i], NULL, &route,tmp) != 0)
       			 perror("Failed to create thread");
	}

	

	for (i=0;i<numofWalkers;i++){
   		 if (pthread_join(road_t[i],NULL) != 0)
		   	 perror("Failed to join thread");
      	}
	pthread_mutex_destroy(&print);
	pthread_mutex_destroy(&count);
	pthread_mutex_destroy(&walk);
	pthread_mutex_destroy(&side);
	pthread_mutex_destroy(&out);
	  return 0;
}
