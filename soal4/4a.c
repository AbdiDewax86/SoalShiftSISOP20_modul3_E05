#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

pthread_t tid[4];
pid_t child;

int A[4][2] = {
    {4, 2},
    {1, 3},
    {2, 5},
    {2, 3}
};
int B[2][5] = {
    {1, 3, 5, 7, 9},
    {2, 4, 6, 8, 10}
};
int *C;
int step = 0;

void *matrix(void *arg){
    int core = step++;
    int i,j,k;
    for(i = core; i<(core+1); i++){
        for(j = 0; j<5; j++){
            for(k=0; k<2; k++){
                C[i*5 + j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(void){
    int i,j;
    
    key_t key = 1234;

    int shmid = shmget(key, sizeof(int)*4*5, IPC_CREAT | 0666);
    C = (int *)shmat(shmid, NULL, 0);

    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 5; j++) 
        {
            C[i*5 + j] = 0;
        }
    }

    //int err;
    i=0;
    for(i = 0; i<4; i++){
        int *p;
        pthread_create(&tid[i],NULL,matrix, (void*)(p));
    }

    for(i=0; i<4; i++){
        pthread_join(tid[i],NULL);
    }

    printf("Matriks 4x2 A:\n");
    for (i=0; i<4; i++){
        for (j=0; j<2; j++)
        {
           printf ("%d ",A[i][j]);
        }
        printf ("\n");
    }  
    printf("Matriks 2x5 B:\n");
    for (i=0; i<2; i++){
        for (j=0; j<5; j++)
        {
           printf ("%d ",B[i][j]);
        }
        printf ("\n");
    }  
    printf("Matriks 4x5 C:\n");
    for (i=0; i<4; i++){
        for (j=0; j<5; j++)
        {
           printf ("%d ",C[i*5 + j]);
        }
        printf ("\n");
    }  
    /*
    while(i<2) // loop sejumlah thread
    {
        err=pthread_create(&(tid[i]),NULL,&matrix,NULL); //membuat thread
        if(err!=0) //cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            printf("\n create thread success\n");
        }
        i++;
    }
    */


    //pthread_join(tid[0],NULL);
	//pthread_join(tid[1],NULL);

	exit(0);
	return 0;
}
