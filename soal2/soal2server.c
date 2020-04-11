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

void add_lobby(connection_t * conn)
{
    int thislobby = -1;
    int thisplayer = -1;
    int i=0;
    int *opponent;
    int *player = conn->sock;
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
    while(!lobby[thislobby].player1 || !lobby[thislobby].player2)
    {
    }
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
