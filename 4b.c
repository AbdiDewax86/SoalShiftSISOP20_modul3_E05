#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

int *C;
int D[20];

pthread_t tid[2];

int sequence(int num){
    if(num > 1)
        return num + sequence(num-1);
    else
    {
        return num;
    }
}

void *convert(void *arg){
    pthread_t id=pthread_self();
    int i,j;
    if(pthread_equal(id,tid[0])){
        printf("test\n");
        for(i=0; i<4; i++){
            for(j=0; j<4; j++){
                D[i*5 + j] = sequence(C[i*5 + j]);
                //printf("%d\n",sequence(C[i*5 + j]));
            }
        }
    }
    if(pthread_equal(id,tid[1])){
        for(i=0; i<4; i++){
            for(j=0; j<4; j++){
                printf("%d ",D[i*5 + j]);
            }
            printf("\n");
        }
    }
}

int main(void){
    key_t key = 1234;
    int i,j;

    int shmid = shmget(key, sizeof(int[4][5]), IPC_CREAT | 0666);
    C = (int *)shmat(shmid, NULL, 0);
    
    printf("Matriks 4x5 C:\n");
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 5; j++) 
        {
            printf("%d ",C[i*5+j]);
        }
        printf("\n");
    }

    int err;
    i=0;
    while(i<2) // loop sejumlah thread
    {
        err=pthread_create(&(tid[i]),NULL,&convert,NULL); //membuat thread
        if(err!=0) //cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            //printf("\n create thread success\n");
        }
        i++;
    }
    pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);

	exit(0);
    return 0;
}