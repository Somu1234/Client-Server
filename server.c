#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define MAX 1024

void server_send(void *socketfd, char *filepath)
{
    FILE *fd = fopen(filepath, "rb");
    puts(filepath);
    printf("SENDING FILE\n");
    if(fd == NULL)
        printf("FILE ERROR\n");
    
    int socket = *(int *)socketfd;
    int c, write_buf[MAX];
    while(!feof(fd))
    {
        int n = fread(write_buf, sizeof(int), MAX, fd);
        write(socket, write_buf, n * sizeof(int));
    }
    fclose(fd);
    puts("SENT FILE");
    close(socket);
}

int main()
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8288);
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
    
    while(1)
    {
        puts("WAITING...");
        struct sockaddr_in client;
        int len = sizeof(struct sockaddr_in);
        int newsocketfd;
        
        newsocketfd = accept(socketfd, (struct sockaddr *)&client, (socklen_t *)&len);
        if(newsocketfd < 0)
        {
            perror("ACCEPT FAIL");
            return 1;
        }
        puts("Connection Accepted....");
        char buffer[MAX];
        /*"./Server/1.pdf"*/
        int n = recv(newsocketfd, buffer, MAX, 0);
        buffer[n] = '\0';
        printf("FILENAME RECEIVED : ");
        puts(buffer);
        char *c = strtok(buffer, "\n"); //remove trailing newline
        char *message = (char*)malloc(100 *sizeof(char));
        sprintf(message, "%s", "./Server/");
        strcat(message, c);
        server_send(&newsocketfd, message);
    }
    return 0;
}
