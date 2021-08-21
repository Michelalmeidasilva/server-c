#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#define _GNU_SOURCE

struct configs {
    char * port_number;
    char * server_path;
};

char * get_current_path(){
    char* buffer;

    if( (buffer=getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        return buffer;
    }

    return 0;
}

struct configs  read_env(char * required_variable){
    char * line = NULL;
    char * server_path;
    char * port_number;

    struct configs config_env;

    size_t len = 0;
    ssize_t read;
    
    FILE * fp = fopen("/home/michelalmeida/Workspace/Faculdade/server-c/server-source/.env", "r");

    if (fp == NULL){
        perror("arquivo .env não encontrado\n");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
       if(strstr(line, "SERVER_PATH") != NULL) {
           char * token = strtok(line, "SERVER_PATH=");
           server_path = token;
       }

       if(strstr(line, "PORT") != NULL) {
           char * token = strtok(line, "PORT=");
           port_number = token;
       }       

    }

    config_env.port_number = port_number;
    config_env.server_path = server_path;

    return config_env;

    exit(EXIT_SUCCESS);
}

