#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define MAX 1024

char filepath[200];
char filename[100];

void client_recv(void *socketfd)
{
    puts(filepath);
    FILE *fd = fopen(filepath, "wb");
    if(fd == NULL)
        printf("FILE ERROR");
    
    int socket = *(int *)socketfd;
    int c, read_buf[MAX];
    while((c = recv(socket, read_buf, sizeof(read_buf), 0)) > 0)
    {   
        fwrite(read_buf, sizeof(int), (c / sizeof(int)), fd);
        printf("Writing..\n");
    }
    fclose(fd);
    close(socket);
}

void *server_send(void *socketfd)
{
    puts(filepath);
    FILE *fd = fopen(filepath, "rb");
    if(fd == NULL)
    {
        printf("FILE ERROR AT PROXY. CONNECTING TO SERVER\n");
        
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(8288);
        server.sin_addr.s_addr = INADDR_ANY;
        int socketfd1 = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd1 == -1)
            puts("SOCKET ERROR");
        if(connect(socketfd1, (struct sockaddr *)&server, sizeof(server)) < 0)
            puts("CONNECT ERROR");
        else
            puts("CONNECTED");
        
        write(socketfd1, filename, strlen(filename) + 1); //Send filename to server;
        puts("WRITE DONE");
        client_recv(&socketfd1);
        
        printf("FILE RECEIVED.\n");
        printf("SENDING\n");
        fd = fopen(filepath, "rb");
        int socket = *(int *)socketfd;
        int c, write_buf[MAX];
        while(!feof(fd))
        {
            int n = fread(write_buf, sizeof(int), MAX, fd);
            write(socket, write_buf, n * sizeof(int));
        }
        fclose(fd);
        close(socket);
    }
    else
    {
        printf("SENDING\n");
        int socket = *(int *)socketfd;
        int c, write_buf[MAX];
        while(!feof(fd))
        {
            int n = fread(write_buf, sizeof(int), MAX, fd);
            write(socket, write_buf, n * sizeof(int));
        }
        fclose(fd);
        close(socket);
    }
}

int main()
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8287);
    server.sin_addr.s_addr = INADDR_ANY;

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1)
        puts("SOCKET ERROR");

    if(bind(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
	puts("BIND FAIL");
	return 1;
    }
    else
    	puts("BIND DONE");

    listen(socketfd, 3);
    
    puts("WAITING...");
    struct sockaddr_in client;
    int len = sizeof(struct sockaddr_in);
    int newsocketfd;

    while(newsocketfd = accept(socketfd, (struct sockaddr *)&client, (socklen_t *)&len))
    {
        puts("Connection Accepted....");
        pthread_t sniffer_thread;
        int *socketptr  = (int *)malloc(sizeof(int *));
        *socketptr = newsocketfd;
        
        char buffer[MAX];
        /*"./Server/1.pdf"*/
        int n = recv(newsocketfd, buffer, MAX, 0);
        buffer[n] = '\0';
        strcpy(filename, buffer);
        printf("FILENAME RECEIVED : ");
        puts(filename);
        char *c = strtok(buffer, "\n"); //remove trailing newline
        sprintf(filepath, "%s", "./Proxy/");
        strcat(filepath, c);
        if(pthread_create(&sniffer_thread, NULL, server_send, (void*)socketptr) < 0)
        {
            perror("THREAD ERROR");
            return 1;
        }
        else
	        puts("HANDLER ASSIGNED");
    }
    return 0;
}
