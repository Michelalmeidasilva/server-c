#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#include <arpa/inet.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT_DEST 8888
#define ADDRESS_HOST "http://localhost"

int main () {
  int sock;
  struct sockaddr_in client;
  int len = sizeof(struct sockaddr_in);
  char msg_write[100], msg_read[1024];
  
  sock = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&client, sizeof(client));

  client.sin_family = AF_INET;
  client.sin_port = htons(PORT_DEST);

  inet_aton(ADDRESS_HOST, &client.sin_addr);

  connect(sock, (struct sockaddr *)&client, sizeof(client));

  do {
    scanf("%s", msg_write);
    strcat(msg_write, "\n");
    write(sock, msg_write, strlen(msg_write) + 1);
    read(sock, msg_read, 1024);
    printf("%s\n", msg_read);

  } while(strncmp(msg_read, "end", 3));

}