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

  #include <pthread.h>   // for threading, link with lpthread
  #include <semaphore.h> 

  #include "helper.h"

  #define PORT_DEFAULT 8000
  #define BUFFER_SIZE 1024
  #define CONNECTION_NUMBER 10
  #define PENDING_CONNECTIONS 10

  /// Código Fonte baseado em [Özgür Hepsa?](https://github.com/ozgurhepsag/Multi-threaded-HTTP-Server)



  int thread_count = 0; // Keeps the number of the threads working simultaneously.
  sem_t mutex; // To control thread_counter.

  void jpeg_handler(int socket, char *file_name) {
      char *buffer;
      int fp;

      struct configs config_env = read_env("SERVER_PATH");

      char * server_path = config_env.server_path;
      char * full_path = (char *)malloc((strlen(server_path) + strlen(file_name)) * sizeof(char));
      
      strcpy(full_path, server_path); // Merge the file name that requested and path of the root folder
      strcat(full_path, file_name);

      if ((fp=open(full_path, O_RDONLY)) > 0) {
        int bytes;
        char buffer[BUFFER_SIZE];

        puts("HTTP/1.0 200 OK ");
        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45, 0);

        while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
       
        write (socket, buffer, bytes); // Send the part of the jpeg to client.
      }
      else  {
          puts("HTTP/1.0 404 Not Found ");
          write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
      }

      free(full_path);
      close(fp);
  }

  void html_handler(int socket, char *file_name){
      char *buffer;

      struct configs config_env = read_env("SERVER_PATH");

      char * server_path = config_env.server_path;

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
          
          fread(buffer, bytes_read, 1, fp); // Read the html file to buffer.
          write (socket, buffer, bytes_read); // Send the content of the html file to client.
          free(buffer);
          
          fclose(fp);
      } else {
          puts("HTTP/1.0 404 Not Found ");
          write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
      }

      free(full_path);
  }

  void *connection_handler(void *socket_desc){

      int request;
      char client_reply[BUFFER_SIZE], *request_lines[3];
      char *file_name;
      char *extension;

      // Get the socket descriptor.
      int sock = *((int *)socket_desc);

      // Get the request.
      request = recv(sock, client_reply, BUFFER_SIZE, 0);

      sem_wait(&mutex);
      thread_count++; 

      // If there is CONNECTION_NUMBER requests at the same time, other request will be refused.
      if(thread_count > CONNECTION_NUMBER) {
          char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>";
          write(sock, message, strlen(message));
          thread_count--; 
          sem_post(&mutex);
          free(socket_desc);
          shutdown(sock, SHUT_RDWR);
          close(sock);
          sock = -1;
          pthread_exit(NULL);
      }
      sem_post(&mutex);

      if (request < 0){
          puts("Recv failed\n");
      }
      else if (request == 0){
          puts("Client disconnected upexpectedly. \n");
      }
      else{
          request_lines[0] = strtok(client_reply, " \t\n");
          if (strncmp(request_lines[0], "GET\0", 4) == 0){
              // Parsing the request header.
              request_lines[1] = strtok(NULL, " \t");
              request_lines[2] = strtok(NULL, " \t\n");

              printf("%s", request_lines[2]);
              if (strncmp(request_lines[2], "HTTP/1.0", 8) != 0){
                  puts("HTTP/1.0 400 Bad Request \n");

                  char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>";
                  write(sock, message, strlen(message));
              } else {
                  char *tokens[2]; // For parsing the file name and extension.

                  file_name = (char *)malloc(strlen(request_lines[1]) * sizeof(char));
                  strcpy(file_name, request_lines[1]);
                  printf("get - %s ", file_name);
                  
                  // Getting the file name and extension
                  tokens[0] = strtok(file_name, ".");
                  tokens[1] = strtok(NULL, "."); 
    
                  if(strcmp(tokens[0], "/favicon") == 0 && strcmp(tokens[1], "ico")) {
                      sem_wait(&mutex);
                      thread_count--;
                      sem_post(&mutex);
                      free(socket_desc);
                      shutdown(sock, SHUT_RDWR);
                      close(sock);
                      sock = -1;
                      pthread_exit(NULL);
                  } else if (tokens[0] == NULL || tokens[1] == NULL){
                      char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. (You need to request to jpeg and html files)</body></html>";
                      write(sock, message, strlen(message));
                  } else {

                      if (strcmp(tokens[1], "html") != 0 && strcmp(tokens[1], "jpeg") != 0){
                          char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. Not Supported File Type (Suppoerted File Types: html and jpeg)</body></html>";
                          write(sock, message, strlen(message));
                      }
                      else{
                          if (strcmp(tokens[1], "html") == 0){
                              sem_wait(&mutex); 
                              html_handler(sock, request_lines[1]);
                              sem_post(&mutex);
                          }
                          else if (strcmp(tokens[1], "jpeg") == 0){
                              sem_wait(&mutex); 
                              jpeg_handler(sock, request_lines[1]);
                              sem_post(&mutex);
                          }
                      }
                      free(extension);
                  }
                  free(file_name);
              }
          }
      }

      sleep(50);
      free(socket_desc);
      shutdown(sock, SHUT_RDWR);
      close(sock);
      sock = -1;
      sem_wait(&mutex);
      thread_count--;
      sem_post(&mutex);
      pthread_exit(NULL);
  }



  // 1. using create(), Create TCP socket.
  // 2. using bind(), Bind the socket to server address.
  // 3 using listen(), put the server socket in a passive mode, where it waits for the client to approach the server to make a connection
  // 4 using accept(), At this point, connection is established between client and server, and they are ready to transfer data.
  // 5 Go back to Step 3.
  int main(int argc, char *argv[]){
      //inicialização de semaforos
      sem_init(&mutex, 0, 1); // Inıtialize the mutex from 1.
      int socket_desc, new_socket, len, *new_sock;
      struct sockaddr_in server, client;

      struct configs config_env = read_env("PORT_NUMBER");

      int port_number;

      if(config_env.port_number != NULL){
          port_number =  atoi(config_env.port_number);
      } else {
          port_number = PORT_DEFAULT;
      }

      socket_desc = socket(AF_INET, SOCK_STREAM, 0);
      
       //retorna -1 se der erro
      if (socket_desc == -1) {
          puts("Não possivel criar socket\n");
          return 1;
      }

      server.sin_family = AF_INET; // familia de protocolo IP
      server.sin_addr.s_addr = INADDR_ANY; // Endereços que estão vindo para mensagens
      server.sin_port = htons(port_number);   /// porta que está sendo recebida e convertida para byte


      int bindReturn = bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
      
      if (bindReturn < 0){
          printf("binding failed%d\n", bindReturn);
          return 1;
      }

      listen(socket_desc,  PENDING_CONNECTIONS);

      printf("Servidor montado no caminho :%s.\n", config_env.server_path);
      printf("Esperando por conexões na porta ... :%d.\nm", port_number);

      len = sizeof(struct sockaddr_in);

      while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *) &len)) {
          pthread_t sniffer_thread;
          new_sock = malloc(1);

          *new_sock = new_socket;

          if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0){
              puts("Não foi possivel criar um novo processo. \n");
              return 1;
          }   
      }


      return 0;
  }