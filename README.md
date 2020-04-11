# SoalShiftSISOP20_modul3_E05
## Soal 2
#### Server Side:
```
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080
void *process(void *ptr);

int player_count=0;

struct Lobby
{
    int *player1;
    int *player2;
} lobby[10];


struct account
{
    char username[16];
    char password[16];
} acc;

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;
```


```
int main(int argc, char const *argv[]) {
    //membuat file untuk menyimpan data account
    FILE *fp;
    fp = fopen("account.txt", "a+");
    char *ptrusr = acc.username;
    char *ptrpsw = acc.password;
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    fclose(fp);
    int server_fd, new_socket, valread;
    connection_t * connection;
    struct sockaddr_in address;
    pthread_t thread;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello;
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int player_count;
    while(1)
    {
        /* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(server_fd, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
    }
    return 0;
}
```

Pada fungsi main, kita membuat socket dan membuat multithread untuk menerima lebih dari 1 client, ketika client telah tersambung ke server, maka akan masuk kedalam fungsi proses.

```
void *process(void *ptr)
{
	char buffer[20] = {0};
    char *ptrchar;
	connection_t * conn;
    char error[] = "INPUT ERROR!\n";
    char exit[] = "Bye!\n";
    int i;

	if (!ptr) pthread_exit(0); 
	conn = (connection_t *)ptr;

	while(1)
    {
        //reset char
		memset(buffer,'\0',20);
        //input choices
        ptrchar = buffer;
        read( conn->sock , ptrchar, 20);
        printf ("Client : %s\n",buffer);
        //if login
        if (strcmp(buffer,"login")==0)
        {
            //login success
            if(login(conn)==1)
            {
                screen2(conn);
            }
            //login failed
        }
        //if register
        else if (strcmp(buffer,"register")==0)
        {
            registerr(conn);
        }
        //error
        else
        {}
    }

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}
```

Pada fungsi proses, server akan menunggu input dari client yaitu login dan register. Jika input client adalah register, maka akan masuk kedalam fungsi register. Dan jika input client adalah login, maka akan masuk kedalam fungsi login.

```
void registerr (connection_t *conn)
{
    int n, chars;
    char username[16] = {0},password[16] = {0};
	char buffer[20] = {0};
    char _register[] = "Register success\n";
    char *ptrchar;

    //buka file
    FILE *fp;
    fp = fopen("account.txt", "a+");
    char *ptrusr = acc.username;
    char *ptrpsw = acc.password;
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    //input username dan password
    ptrchar = username;
    read( conn->sock , ptrchar, 20);
    ptrchar = password;
    read( conn->sock , ptrchar, 20);
 
    fflush(stdin);
    
    //input account ke file
    strcpy(ptrusr,username);
    strcpy(ptrpsw,password);
    chars = fwrite(&acc, sizeof(acc), 1, fp);
    fclose(fp);
    fp = fopen("account.txt", "rb");
 
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    if (chars == 1){
        printf ("List account :\n");
        while( fread(&acc, sizeof(acc), 1, fp) == 1 )
        {
            printf ("username : %s\n",acc.username);
            printf ("password : %s\n",acc.password);
        }
    }
    ptrchar = _register;
    send(conn->sock , ptrchar , strlen(ptrchar) , 0 );
    fclose(fp);
}
```

Pada fungsi register, server menunggu input username dan password dari client. Kemudian akan membuka file ‘account.txt’ dan menambahkan account baru kedalam file tersebut dan mengirimkan sinyal bahwa register telah berhasil kepada client.

```
int login(connection_t *conn)
{
    int flag = 0;
    //buka file
    FILE *fp;
    fp = fopen("account.txt", "rb");
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    char _loginsuccess[] = "Login success";
    char _loginfailed[] = "Login failed";
    char username[16] = {0},password[16] = {0};
	char buffer[20] = {0};
    char *ptrchar;
    
    //input username dan password
    ptrchar = username;
    read( conn->sock , ptrchar, 20);
    ptrchar = password;
    read( conn->sock , ptrchar, 20);
    
    //check account di file
    while( fread(&acc, sizeof(acc), 1, fp) == 1 )
    {
        //login success
        if (strcmp(acc.username,username)==0 && strcmp(acc.password,password)==0)
        {
            printf("Auth success\n");
            flag = 1;
            fclose(fp);
            ptrchar = _loginsuccess;
            send(conn->sock , ptrchar , strlen(ptrchar) , 0 );
            return 1;
        }
    }
    fclose(fp);
    //login failed
    ptrchar = _loginfailed;
    send(conn->sock , ptrchar , strlen(ptrchar) , 0 );
    if (flag==0) 
    {
        printf ("Auth failed\n");
        ptrchar = _loginfailed;
        send(conn->sock , ptrchar , strlen(ptrchar) , 0 );
    }
    return 0;
}
```

Pada fungsi login, server akan menunggu input username dan password dari client. Kemudian akan membuka dan mencek file ‘account.txt’ apakah inputan username dan password dari client telah tercatat di file tersebut. Jika sudah, maka akan dikirimkan sinya bahwa login success, namun jika tidak, maka dikirimkan sinya login failed.

Jika login success, maka akan memasuki fungsi screen 2.

```
void screen2(connection_t *conn)
{
    char buffer[20] = {0};
    char *ptrchar;
    while (1)
    {
        memset(buffer,'\0',20);
        ptrchar = buffer;
        read( conn->sock , ptrchar, 20);
        printf ("Client : %s\n",buffer);
        if (strcmp(buffer,"find")==0)
        {
            add_lobby(conn);
            memset(buffer,'\0',20);
        }
        else if (strcmp(buffer,"logout")==0)
        {
            printf("Logout success\n");
            memset(buffer,'\0',20);
            break;
        }
    }
}
```

Pada screen2, server akan menunggu input dari client (find atau logout). Jika client memberikan input logout, maka akan kembali ke fungsi process, namun jika client memberikan input find, maka akan masuk fungsi add_lobby.

```
//menambahkan player kedalam lobby
void add_lobby(connection_t * conn)
{
    int thislobby = -1;
    int thisplayer = -1;
    int i=0;
    int *opponent;
    int *player = conn->sock;
    //mencari lobby yang kosong
    while(i<10)
    {
        if(!lobby[i].player1)
        {
            thislobby = i;
            thisplayer = 1;
            lobby[i].player1 = player;
            break;
        }
        else if(!lobby[i].player2)
        {
            thislobby = i;
            thisplayer = 2;
            lobby[i].player2 = player;
            break;
        }
        i++;
    }
    //printf ("player in lobby : %d\n",player_count);
    char inlobby [] = "Kamu telah masuk kedalam lobby, silahkan tunggu..\n";
    char *ptrchar = inlobby;
    send(conn->sock , ptrchar , strlen(ptrchar) , 0 );
    printf ("Lobby %d in waiting..\n",thislobby);
    //menunggu kedua player berada didalam satu lobby
    while(!lobby[thislobby].player1 || !lobby[thislobby].player2)
    {
    }
    //kedua player telah didalam lobby
    if (thisplayer == 1){
        opponent = lobby[thislobby].player2;
    }
    else if (thisplayer == 2){
        opponent = lobby[thislobby].player1;
    }
    char play [] = "play";
    ptrchar = play;
    send(player , ptrchar , strlen(ptrchar) , 0 );
    printf ("Game berjalan..\n");
    ingame(player, opponent);
    lobby[thislobby].player1=-1;
    lobby[thislobby].player2=-1;
}
```

Pada fungsi add_lobby, dilakukan looping untuk mencari lobby mana yang belum terisi oleh 2 player. Ketika telah menemukan lobby, socket player akan disimpan kedalam struct player. Kemudian pemain akan menunggu pemain lain didalam lobby tersebut. Ketika pemain lain sudah memasuki lobby, server akan mencatat socket pemain dan socket musuhnya. Setelah itu, game akan dimulai dan masuk ke fungsi ingame.

```
void ingame(int *player, int *opponent)
{
    char attacked[] = "Attacked";
    char selesai[] = "Selesai";
    //printf ("sockplayer = %d dan opp = %d",player,opponent);
    while(1)
    {
        char *ptrchar;
        char buffer[20];
        read(player , buffer, 20);
        printf ("buffer = %s\n",buffer);
        if(strcmp(buffer,"Selesai")==0)
        {
            memset(buffer, '\0', 20);
            printf("Game selesai!\n");
            ptrchar = selesai;
            send(opponent, ptrchar, strlen(ptrchar), 0);
            break;
        }
        else if (strcmp(buffer,"Attack")==0)
        {
            memset(buffer, '\0', 20);
            printf ("client attack!\n");
            ptrchar = attacked;
            send(opponent , ptrchar , strlen(ptrchar) , 0 );
        }
    }
}
```

Pada fungsi ingame, dilakukan loop selama permainan masih berlangsung. Server akan terus menerima sinyal dari client apakah itu sinyal attack ataupun sinyal bahwa client telah kalah. Jika sinyal yang diterima adalah attack, maka server akan mengirimkan sinyal tersebut kepada musuhnya. Begitu juga dengan sinyal selesai yang menandakan bahwa client telah mati dan musuh sebagai pemenangnya. Sinyal ini menandakan bahwa game telah selesai. Dan kembali ke screen2.

#### Client Side:
```
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#define PORT 8080

int kbhit(void);
void changemode(int dir);
int game = 0;

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello;
    char ask[20];
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    int i;
    hello=ask;
    while (1)
    {
        screen1(sock);
        //printf("Message dari server diterima\n");
    }
    
    return 0;
}
```

Pada fungsi main, kita akan membuat client agar terhubung dengan server. Setelah terhubung, maka akan masuk kedalam fungsi screen1.

```
void screen1 (int sock)
{
    char *ptrchar;
    //int valread;
    char buffer[30],chrsend[20];
    char username[16],password[16];
    while(1)
    {
        memset(buffer,'\0',30);
        memset(chrsend, '\0', 20);
        //printf ("%s\n",chrsend);
        printf ("1.login\n2.register\nChoices : ");
        scanf("%s",&chrsend);
        ptrchar=chrsend;
        send(sock , ptrchar , strlen(ptrchar) , 0 );
        if (strcmp(chrsend,"login")==0)
        {
            scanf ("%s",username);
            ptrchar = username;
            send(sock , ptrchar , strlen(ptrchar) , 0 );
            memset(username, '\0',16);
            scanf ("%s",password);
            ptrchar = password;
            send(sock , ptrchar , strlen(ptrchar) , 0 );
            memset(password,'\0',16);
            read( sock , buffer, 30);
            if (strcmp(buffer,"Login success")==0)
            {
                printf ("Login success\n");
                screen2(sock);
            }
            else{
                printf ("Login failed\n");
            }
        }
        else if (strcmp(chrsend,"register")==0)
        {
            scanf ("%s",username);
            ptrchar = username;
            send(sock , ptrchar , strlen(ptrchar) , 0 );
            memset(username, '\0',16);
            scanf ("%s",password);
            ptrchar = password;
            send(sock , ptrchar , strlen(ptrchar) , 0 );
            memset(username,'\0',16);
            read( sock , buffer, 30);
            printf("%s",buffer);
            if (strcmp(buffer,"Register success\n")==0)
            {
                memset(buffer,'\0',30);
            }
        }
        else{
            printf("Input error!\n");
        }
    }
        //valread = read( sock , buffer, 30);
        //return 0;

}
```

Pada screen 1, client akan memberikan pilihan mana yang ingin dipilih (login atau register). Jika client memilih register, maka client akan memberikan username dan password kepada server untuk didaftarkan. Jika client memilih login, maka client juga akan memberikan username dan password kepada server, jika server memberikan sinyal login success, maka client akan masuk ke screen2, jika tidak maka akan tetap berada di screen1.

```
int screen2 (int sock)
{
    while(1)
    {
        char *ptrchar;
        char chrsend[20],buffer[50];
        printf ("1.find match\n2.logout\nChoices : ");
        scanf("%s",&chrsend);
        ptrchar = chrsend;
        send(sock,ptrchar,strlen(ptrchar),0);
        if (strcmp(chrsend,"find")==0)
        {
            memset(chrsend,'\0',20);
            read( sock , buffer, 50);
            printf ("%s",buffer);
            memset(buffer,'\0',50);
            read( sock , buffer, 30);
            if (strcmp(buffer,"play")==0)
            {
                printf ("Kamu telah menemukan lawan, silahkan bermain!\n");
                memset(buffer,'\0',50);
                game = 1;
                play(sock);
            }
        }
        else if (strcmp(chrsend,"logout")==0)
        {
            printf("Logout success\n");
            memset(chrsend,'\0',20);
            break;
        }
    }

}
```

Pada screen2, client akan memberikan pilihan kepada server (find match atau logout). Jika client memberikan sinyal logout, maka akan kembali ke screen1. Namun jika client mengirimkan sinyal findmatch, maka client akan dimasukkan kedalam lobby dan menunggu sinyal dari server apakah client telah mendapatkan lawan dan akan bermain. Kemudian masuk ke fungsi play.

```
void *attacked(void *ptr)
{
    int health = 100;
    //printf ("sebelum\n");
    int sock = *(int*)ptr;
    //printf ("sock = %d\n",sock);
    //printf ("masuk thread\n");
    char selesai[] = "Selesai";
    while(game == 1)
    {
        char *ptrchar;
        char buffer[20];
        read( sock , buffer, 30);
        printf ("buffer = %s\n",buffer);
        if (strcmp(buffer, "Attacked")==0)
        {
            memset(buffer, '\0', 20);
            health = health - 10;
            printf ("health = %d\n",health);
        }
        else if (strcmp(buffer, "Selesai") == 0)
        {
            memset(buffer, '\0', 20);
            printf ("You win!\n");
            game = 0;
            break;
        }
        if (health <= 0)
        {
            printf ("You lose!\n");
            ptrchar = selesai;
            send(sock,ptrchar,strlen(ptrchar),0);
            read( sock , buffer, 30);
            memset(buffer, '\0', 20);
            game = 0;
            break;
        }
    }
}

void play(int sock)
{
    //printf ("socket = %d\n",sock);
    pthread_t thread;
    changemode(1);
    if (pthread_create(&thread, 0, attacked, (void *)&sock) < 0)
    {
        perror("could not create thread");
        return;
    }
    char hit;
    char *attack = "Attack";
    char *selesai = "Selesai";
    while (game == 1){
        if (!kbhit()){
            if (game == 0)
            {
                break;
            }
            hit = getchar();
            if(hit == ' ' && game==1)
            {
                printf("HIT!!!\n");
                send(sock,attack,sizeof(attack)+1,0);
            }
        }
    }
    send(sock,selesai,sizeof(selesai)+1,0);
    changemode(0);
}
```

Karena pada fungsi play dibutuhkan 2 pekerjaan dalam waktu yang sama, yaitu memberikan 'HIT' kepada musuh dan juga menerima serangan dari musuh, maka pada fungsi ini dibuatlah thread 'attacked'. Pada fungsi play, digunakan fungsi changemode() dan kbhit() agar pemain dapat bermain permainan ini dengan menekan 'Space'. Jika pemain menekan 'Space', maka client akan mengirimkan sinyal 'HIT' kepada server agar server mengirimkan sinyal tersebut kepada musuh.
Pada fungsi 'Attacked', client akan menerima sinyal dari server, jika sinyal yang diterima merupakan sinyal 'attack', maka health pemain akan -10. Dan jika health pemain <= 0, maka client akan mengirimkan sinyal game selesai kepada server yang menandakan pemain telah kalah. Jika sinyal yang diterima merupakan sinyal 'selesai', menandakan bahwa musuh telah mati dan pemain telah memenangkan permainan. Kemudian kembali ke screen2.

## Soal 4
#### Soal 4a.
```
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
```
Dalam implementasi soal 4, terdapat matriks 4x2 A dan matriks 2x5 B. Kedua matriks tersebut sudah diinisialisasi dan akan dimasukkan ke matriks C 4x5. Perkalian matriks dilakukan melalui thread matrix. Pemanggilan thread ini dilakukan 4 kali, dan setiap pemanggilan mengisi 1 baris matriks. Matriks C didefinisikan dalam bentuk matriks 1 dimensi berukuran 20 (dari 4x5), agar dapat di passing melalui shared memory, dan dipakai untuk implementasi soal 4b. Untuk output, matriks A, B, dan C diprint.

#### Soal 4b.
```
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
```
Dalam implementasi soal 4b, hal yang pertama dilakukan adalah print matriks C. Dalam implementasi ini, terdapat matriks D untuk menyimpan nilai deret aritmatika ke-n untuk setiap elemen dalam matriks C. Terdapat fungsi sequence untuk menghitung deret tersebut. Untuk thread, terdapat thread untuk melakukan operasi tersebut dan memasukkannya ke matriks D, dan thread untuk print matriks D. Thread ini langsung dipanggil untuk melakukan operasi dan mengeluarkan output

#### Soal 4c.
```
#include  <fcntl.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <sys/stat.h>
#include  <termios.h>
#include  <unistd.h>                             

#define INPUT_END 1 //write
#define OUTPUT_END 0//read

int main()
{
    pid_t pid;
    int fd[2];
    if (pipe(fd) < 0) exit(1);
    pid=fork();
    if (pid==0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", NULL);
        printf("Gagal mengeksekusi1\n");
        exit(1);
    }
    else
    {
        pid = fork();
        if(pid==0)
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
            execlp("wc", "wc", "-l", NULL);
            printf("Gagal mengeksekusi2\n");
            exit(1);
        }
        else
        {
            int status;
            close(fd[0]);
            close(fd[1]);
            waitpid(pid, &status, 0);
        }
        
    }
    
}   
```

Dalam implementasi soal 4c, kita mulai dengan membuat pipe dan kemudian membuat fork. Child fork digunakan sebagai command output yaitu fungsi ls, sehingga kita harus mengubah standart output file descriptor (1) pada pipe write fd[1]. Parent fork digunakan sebagai command input yaitu fungsi wc, sehingga kita harus mengubah standart input file descriptor (0) pada pipe read fd[0]. Kita mengubah file descriptor tersebut menggunakan fungsi dup2() dimana akan membuat duplicate dari file descriptor pipe sehingga file desciptor tetap terbuka untuk menjalankan fungsi exec.
