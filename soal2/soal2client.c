#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

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
            read( sock , buffer, 50);
            printf ("%s",buffer);
            memset(buffer,'\0',50);
            read( sock , buffer, 30);
            if (strcmp(buffer,"play")==0)
            {
                printf ("Kamu telah menemukan lawan, silahkan bermain!\n");
            }
        }
        else if (strcmp(chrsend,"logout")==0)
        {
            screen1(sock);
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
        /*valread = read( sock , buffer, 30);
        printf ("%s",buffer);
        memset(buffer, '\0', 30);*/
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
            memset(username,'\0',16);
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
