#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#include <arpa/inet.h> 
#include <unistd.h>   
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <unistd.h>

#include <pthread.h> 
#include <semaphore.h>

#include "messages.h"
#include "errors.h"
#include "server-http-1.1.h"
#include "env.h"

int thread_count = 0; 
int socket_counter = 0;
sem_t mutex;          

void setup_server(int * socket_desc,  struct sockaddr_in server){
  sem_init(&mutex, 0, 1); 

  * socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if (* socket_desc == -1){
    perror("Não possivel criar socket\n");
    exit(SOCKER_ERROR);
  }

  server.sin_family = AF_INET;          // familia de protocolo IP
  server.sin_addr.s_addr = INADDR_ANY;  // Endereços que estão vindo para mensagens
  server.sin_port = htons(PORT_DEFAULT); /// porta que está sendo recebida e convertida para byte

  if (bind(* socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("error binding");
    exit(BINDING_ERROR);
  }

  listen(* socket_desc, 1);
  printf("Esperando por conexões na porta ... :%d.\n", PORT_DEFAULT);
}

/**
 * @return  retorna 0 se for um metodo get
 */
int isMethodGet(char * method){
  return strncmp(method, "GET\0", 4) == 0;
}


/*flag para versao http 1.0.
    VERSION_HTTP1_0 = flag para versão do HTTP 1.0;     
    VERSION_HTTP1_1 = flag para versão do HTTP 1.1;          
  retorna 0 se o protocolo for do tipo passado como argumento. */
int isProtocolHttp(char * protocol, char * version){
  return strncmp(protocol, version, 8) == 0;
}


/* checagem de extensão para extensões.
    HTML_TYPE = flag para extensão html;     
    JPEG_TYPE = flag para extensão jpeg;          
  @return  0 se a extensão for do tipo passado como argumento. */
int checkFileExtension(char * file_name, char * extension_type){
  char * token[2];
  
  token[0] = strtok(file_name, ".");
  token[1] = strtok(NULL, ".");

  return strcmp(token[1],  extension_type) == 0;
}


/*
  Enviar uma mensagem ao cliente: 
  header = header to send protocol method;
  connection_close = general header controls whether or not the network connection stays open after the current transaction finishes;
  media_type = is a two-part identifier for file formats and format contents transmitted on the Internet; */
void sendResponse(char *header, char *conection, char *media_type, char *html_render, int sock){
  char *message ;

  strcat(message, header);
  strcat(message, conection);
  strcat(message, media_type);
  strcat(message, html_render);
  
  puts(header);
  write(sock, message, strlen(message));
}


/**
 * Função para extrair a header da request
 * 
 * @return  request_header   Struct do tipo Request .
 * Request attributes: [ Method, Resource, Protocol ]
 */
Request  request_header( char client_reply[BUFFER_SIZE]){
  Request request;

  request.method = strtok(client_reply, " \t\n");
  request.resource = strtok(NULL, " \t");
  request.protocol = strtok(NULL, " \t\n");

  printf("Method:%s\tResource:%s\tProtocol:%s\n", request.method, request.resource, request.protocol);

  return request;
}

/*
 * Manipulador de arquivos, a função verifica a extensão do arquivo e manipula o `FILE` para carregar e mandar ao cliente a resposta.
 * Caso o arquivo não seja encontrado, a mensagem de NOT FOUND 404 é enviada.
 */

void file_handler(int socket, char *file_name){
  char *full_path = (char *)malloc((strlen(PATH) + strlen(file_name)) * sizeof(char));
  char *buffer;

  strcpy(full_path, PATH); 
  strcat(full_path, file_name);

  if(checkFileExtension(file_name, HTML_TYPE)){
    FILE *fp = fopen(full_path, "r");

    if(fp != NULL){
      fseek(fp, 0, SEEK_END); 
      long bytes_read = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      puts(SUCCESSFULL_MESSAGE_TEXT);

      send(socket, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0); 

      buffer = (char *)malloc(bytes_read * sizeof(char));

      fread(buffer, bytes_read, 1, fp);  
      write(socket, buffer, bytes_read); 
      free(fp);
    }
  } else if(checkFileExtension(file_name, JPEG_TYPE)){
    int fp = open(full_path, O_RDONLY);

    if (fp > 0) {
      buffer[BUFFER_SIZE];

      int bytes;

      puts(SUCCESSFULL_MESSAGE_TEXT);

      send(socket, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45, 0);
      
      while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
      
      write(socket, buffer, bytes); // Send the part of the jpeg to client.
    }
  } else {
    puts(NOT_FOUND_MESSAGE_TEXT);
    sendResponse(NOT_FOUND_MESSAGE_TEXT, "Connection: close\r\n", "Content-Type: text/html\r\n\r\n", NOT_FOUND_MESSAGE_HTML, socket);
  }

  free(full_path);
}


void close_connection(int * sock){
  shutdown(* sock, SHUT_RDWR);
  close(* sock);
  * sock = -1;
  sem_wait(&mutex);
  sem_post(&mutex);
  printf("\n---------------------------\n");
  pthread_exit(NULL);
}


void *connection_handler(void * sock_desc){
  char *file_name, client_reply[BUFFER_SIZE];
  int request, sock = *((int *) sock_desc);

  printf("\n---------------------------\n");
  while(1){
    request = recv(sock, client_reply, BUFFER_SIZE, 0);

    if (request < 0){
      perror("Error 11: Recv failed, msg: ");
      close_connection(&sock);
    } else if (request == 0){
      perror("Error 44: Client disconnected upexpectedly, msg:");
      close_connection(&sock);
    } 
    
    if (request > 0) {
      Request request = request_header(client_reply);
      
      if (isMethodGet(request.method) && isProtocolHttp(request.protocol, VERSION_HTTP1_0) || isProtocolHttp(request.protocol, VERSION_HTTP1_1)){      
        file_name = (char *)malloc(strlen(request.resource) * sizeof(char));

        strcpy(file_name, request.resource);

        if (file_name != NULL) {
          sem_wait(&mutex);
          file_handler(sock, request.resource);
          sem_post(&mutex);
        } else {
          sendResponse(BAD_REQUEST_MESSAGE_TEXT, "Connection: close\r\n", "Content-Type: text/html\r\n\r\n", BAD_REQUEST_MESSAGE_HTML, sock);
        }
    
        free(file_name);
      } else {
        sendResponse(METHOD_NOT_ALLOWED_MESSAGE_TEXT, "Connection: close\r\n", "Content-Type: text/html\r\n\r\n", METHOD_NOT_ALLOWED_MESSAGE_HTML, sock);
      }
    }
  }

  close_connection(&sock);
}

int running_server(int socket_desc){
  struct sockaddr_in client;
  int len = sizeof(struct sockaddr_in);
  int new_socket, *new_sock;

  while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&len)){
    pthread_t sniffer_thread;

    new_sock = malloc(1);
    *new_sock = new_socket;
    
    if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *) new_sock) < 0){
      perror("Não foi possivel criar um novo processo.\n");
      exit(CREATE_PROCESS_ERROR);
    }  
  }

  free(new_sock);
}

int main(int argc, char *argv[]){
  int socket_desc;
  struct sockaddr_in server, client;

  setup_server(&socket_desc, server);
  listen(socket_desc, 1);
  running_server(socket_desc);
  
  return 0;
}