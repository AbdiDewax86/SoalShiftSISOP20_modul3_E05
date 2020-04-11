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


void changemode(int dir){
  static struct termios oldt, newt;
 
  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void){
  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}
