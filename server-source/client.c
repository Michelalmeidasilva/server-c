#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#include <arpa/inet.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server-http-1.1.h"

#define PORT_DEST 8080
#define ADDRESS_HOST "http://localhost"

int main () {
  int sock;
  struct sockaddr_in client;
  int len = sizeof(struct sockaddr_in);
  char * msg_write , * msg_read;
  
  Request * sendData = (Request *) malloc(sizeof(Request));
 

  sock = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&client, sizeof(client));

  client.sin_family = AF_INET;
  client.sin_port = htons(PORT_DEST);

  inet_aton(ADDRESS_HOST, &client.sin_addr);

  connect(sock, (struct sockaddr *)&client, sizeof(client));

  printf("Digite end para sair\n");
  char * resource ; 

  do {
    resource =(char*) malloc(100 * sizeof(char));
    msg_write =(char*) malloc(300 * sizeof(char));
    msg_read =(char*) malloc(1024 * sizeof(char));
    
    printf("\nDigite o arquivo que voce busca no servidor: ");
    scanf("%s", resource);

    strcat(msg_write, "GET ");
    strcat(msg_write, "/");
    strcat(msg_write, resource);
    strcat(msg_write, "\t\n");
    strcat(msg_write, " HTTP/1.1\t\n");

    write(sock, msg_write, strlen(msg_write) + 1);

    printf("\nEnviando a mensagem: %s\n", msg_write);

    read(sock, msg_read, 1024);
    printf("%s\n", msg_read);

  } while(strncmp(resource, "end", 3));
}
