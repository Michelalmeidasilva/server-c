#include <stdio.h>
#include <string.h> // for strlen
#include <stdlib.h>

#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <unistd.h>

#include <pthread.h> // for threading, link with lpthread
#include <semaphore.h>

#include "errors.h"
#include "helper.h"
#include "queue.h"

#define PORT_DEFAULT 8000
#define BUFFER_SIZE 1024
#define CONNECTION_NUMBER 5
#define PENDING_CONNECTIONS 10
#define QUEUE_SIZE 10
#define LIMIT_CLIENTS 5

struct arg_struct {
    int sock;
    int threadId;
};

typedef struct{
  int value;
} Request;

typedef struct {
  int address;
  pthread_t thread;
  Queue  * queue_requests;
  struct arg_struct *args;
} Client;


Request * enqueueRequest(Queue * queue_clients, int value){
  Request *request = (Request *)malloc(sizeof(Request));
  enqueue(queue_clients, request);
  request->value = value;
  return request;
}

int thread_count = 0; // Keeps the number of the threads working simultaneously.
sem_t mutex;          // To control thread_counter.
int socket_counter = 0;


void html_handler(int socket, char *file_name){
  char *buffer;

  struct configs config_env = read_env("SERVER_PATH");

  char *server_path = config_env.server_path;

  char *full_path = (char *)malloc((strlen(server_path) + strlen(file_name)) * sizeof(char));

  FILE *fp;

  strcpy(full_path, server_path); // Merge the file name that requested and path of the root folder
  strcat(full_path, file_name);

  fp = fopen(full_path, "r");

  if (fp != NULL){
    fseek(fp, 0, SEEK_END); // Find the file size.
    long bytes_read = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    puts("HTTP/1.0 200 OK ");

    send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0); // Send the header for succesful respond.
    buffer = (char *)malloc(bytes_read * sizeof(char));

    fread(buffer, bytes_read, 1, fp);  // Read the html file to buffer.
    write(socket, buffer, bytes_read); // Send the content of the html file to client.
    free(buffer);

    fclose(fp);
  }
  else{
    puts("HTTP/1.0 404 Not Found ");
    write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
  }

  free(full_path);
}


void *connection_handler(void *arguments){
  struct arg_struct *args = (struct arg_struct *) arguments;

  int sock = args->sock;

  // printf("sera??????? %d \n", args->sock);
  // // queue_client_requests

  // int request;
  // char client_reply[BUFFER_SIZE], *request_lines[3];
  // char *file_name;
  char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>\n";
  write(sock, message, strlen(message));
  close(sock);
  // // Get the socket descriptor.
  // int sock = *((int *)args->arg1);

  // // Get the request.
  // request = recv(sock, client_reply, BUFFER_SIZE, 0);

  // sem_wait(&mutex);
  // thread_count++;

  // // If there is CONNECTION_NUMBER requests at the same time, other request will be refused.
  // if (thread_count > CONNECTION_NUMBER)
  // {
    // write(sock, message, strlen(message));
  //   char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>\n";
  //   thread_count--;
  //   sem_post(&mutex);
  //   free(socket);
  //   shutdown(sock, SHUT_RDWR);
  //   close(sock);
  //   sock = -1;
  //   pthread_exit(NULL);
  // }

  // sem_post(&mutex);

  // printf("Request: %d\n", request);

  // if (request < 0)
  // {
  //   puts("Recv failed\n");
  // }

  // if (request == 0)
  // {
  //   puts("Client disconnected upexpectedly. \n");
  // }

  // request_lines[0] = strtok(client_reply, " \t\n");

  // if (strncmp(request_lines[0], "GET\0", 4) == 0 && strncmp(request_lines[2], "HTTP/1.0", 8) == 0)
  // {
  //   // Parsing the request header.
  //   request_lines[1] = strtok(NULL, " \t");
  //   request_lines[2] = strtok(NULL, " \t\n");

  //   char *tokens[2]; // For parsing the file name and extension.

  //   file_name = (char *)malloc(strlen(request_lines[1]) * sizeof(char));
  //   strcpy(file_name, request_lines[1]);
  //   printf("get - %s ", file_name);

  //   // Getting the file name and extension
  //   tokens[0] = strtok(file_name, ".");
  //   tokens[1] = strtok(NULL, ".");

  //   if (tokens[0] != NULL || tokens[1] != NULL)
  //   {
  //     if (strcmp(tokens[1], "html") == 0){
  //         sem_wait(&mutex);
  //         html_handler(sock, request_lines[1]);
  //         sem_post(&mutex);
  //     }
  //     else{
  //       char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request.(Supported File Types: html)</body></html>";
  //       write(sock, message, strlen(message));
  //     }
  //   }
  //   else
  //   {

  //     char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. (You need to request to jpeg and html files)</body></html>";
  //     write(sock, message, strlen(message));
  //   }
  //   free(file_name);
  // }
  // else
  // {
  //   puts("HTTP/1.0 400 Bad Request \n");

  //   char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>";
  //   write(sock, message, strlen(message));
  // }

  //   sleep(50);
  // free(file_name);


  // free(socket);
  // shutdown(sock, SHUT_RDWR);
  // close(sock);
  // sock = -1;
  // sem_wait(&mutex);
  // sem_post(&mutex);
  // thread_count--;
  // pthread_exit(NULL);
}


/**
 * @return  -1 se a requisição não for de nenhum cliente, caso haja ele retorna o endereço desse cliente.
 */
int contains(Client * clients, int received_s_addr){
  for(int i = 0; i< LIMIT_CLIENTS; i++){
    if(clients[i].address == received_s_addr){
      return i;
    } 
  }

  return -1;
}


/**
 * Função para instanciação do vetor de clientes.
 *
 * @return  vetor de clientes   Struct do tipo Client .
 */
Client * createClients( Client * clients){
  clients= (Client *) malloc(LIMIT_CLIENTS * sizeof (Client ));

  for(int i = 0; i < LIMIT_CLIENTS; i++){
    clients[i].thread = 0;
    clients[i].address = 0;
    clients[i].queue_requests =(Queue *) queue_alloc(QUEUE_SIZE);
  }
  return clients;
}


int main(int argc, char *argv[]){
  Client * clients = createClients(clients);
  sem_init(&mutex, 0, 1); 
	// pthread_t threads[LIMIT_CLIENTS];
  int socket_desc, new_socket, len, *new_sock, index;

  struct sockaddr_in server, client;
  struct configs config_env = read_env("PORT_NUMBER");

  int port_number;
  if (config_env.port_number != NULL){
    port_number = atoi(config_env.port_number);
  } else {
    port_number = PORT_DEFAULT;
  }

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1){
    puts("Não possivel criar socket\n");
    return SOCKER_ERROR;
  }

  server.sin_family = AF_INET;          // familia de protocolo IP
  server.sin_addr.s_addr = INADDR_ANY;  // Endereços que estão vindo para mensagens
  server.sin_port = htons(port_number); /// porta que está sendo recebida e convertida para byte

  int bindReturn = bind(socket_desc, (struct sockaddr *)&server, sizeof(server));

  if (bindReturn < 0){
    printf("binding failed %d\n", bindReturn);
    return BINDING_ERROR;
  }

  listen(socket_desc, 1);

  printf("Esperando por conexões na porta ... :%d.\nm", port_number);

  len = sizeof(struct sockaddr_in);

  while (1){

    if(socket_counter < LIMIT_CLIENTS){
      new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&len);
    }
    
    int indexClient = contains(clients, client.sin_addr.s_addr);

    if(indexClient >= 0){
      if (pthread_create(&clients[socket_counter].thread, NULL, connection_handler, (void *) clients[socket_counter].args) < 0){
          puts("Não foi possivel criar um novo processo. \n");
          return 1;
      } 
    } else {
      pthread_t sniffer_thread;

      clients[socket_counter].thread = sniffer_thread;
      clients[socket_counter].address = client.sin_addr.s_addr;
      
      new_sock = malloc(1);
      *new_sock = new_socket;

      struct arg_struct *args = malloc(sizeof(struct arg_struct));
      args->sock = new_socket;
      args->threadId = 100;
      clients[socket_counter].args = args;

      // printf("%d -----");

      if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)args) < 0){
        puts("Não foi possivel criar um novo processo. \n");
        return 1;
      } 

      socket_counter++;
    }
  }

  printf("------------------------------------\n\n\n\n\n\n");
  for(int i = 0; i < LIMIT_CLIENTS; i++){
    printf("CLIENTE %d\n", i);
    printf("clients[i]->address: %d\n", clients[i].address);
    for(int j = 0; j < QUEUE_SIZE; j++){
      Request * item  = (Request *) dequeue(clients[i].queue_requests);
      printf("item: %d\n ",  item->value );
    }
  }
  
  
  free(new_sock);
  free(clients);
  return 0;
}