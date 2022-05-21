#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1024

void client_recv(void *socketfd, char *filepath)
{
    FILE *fd = fopen(filepath, "wb");
    if(fd == NULL)
        printf("FILE ERROR");
    
    int socket = *(int *)socketfd;
    int c, read_buf[MAX];
    while((c = recv(socket, read_buf, sizeof(read_buf), 0)) > 0)
    {   
        fwrite(read_buf, sizeof(int), (c / sizeof(int)), fd);
    }
    fclose(fd);
    close(socket);
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

    if(connect(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("CONNECT ERROR");
		return 1;
	}
	else
		puts("CONNECTED");
    
    char message[MAX];

    printf("ENTER FILENAME : \n");
    fgets(message, MAX, stdin);
    puts(message);
    write(socketfd, message, strlen(message) + 1);
    char *c = strtok(message, "\n"); //remove trailing newline
    char *filepath = (char*)malloc(100 *sizeof(char));
    sprintf(filepath, "%s", "./Client/");
    strcat(filepath, message);
    puts(filepath);
    client_recv(&socketfd, filepath);

    return 0;
}
