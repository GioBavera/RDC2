#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "serverpi.h"
#include "serverausftp.h"



int is_valid_command(const char *command) {
    int i = 0;

    while (valid_commands[i] != NULL) {
        if (strcmp(command, valid_commands[i]) == 0) {
            return arg_commands[i];
        }
        i++;
    }
    return -1; // Comando no valido
}

/*
    * Este codigo recepciona un comando (recive) desde el socket descriptor sd. 
    * recv es el que hace efectivamente la recepcion de datos. Llama definida en syssocket. 
    * Si esto falla, hacemos un return 1.
*/
int recv_cmd(int sd, char *operation, char *param) {    // sd = socket descriptor, es local.
    char buffer[BUFSIZE];
    char *token;
    int argsNumber;

    if (recv(sd, buffer, BUFSIZE, 0) < 0) {
        fprintf(stderr, "Error: no se pudo recibir el \n");
        return 1;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    token = strtok(buffer, " ");
    // strtok tiene un comportamiento inicial pero en adelante actua de forma distinta.
    if (token == NULL || strlen(token) < 3 || (argsNumber == is_valid_command(token)) < 0) {
        fprintf(stderr, "Error: comando no valido\n");
        return 1;
    } 

    strcpy(operation, token);

    if(!argsNumber){
        return 0;  // Si no hay argumentos, termina la funcion.
    }
    token = strtok(NULL, " ");
    // Aca le indica que siga trabajando con el mismo buffer. Si le paso uno nuevo, da otro token.
    // Utiliza static internamente para saber como actua.
    #if DEBUG 
    printf("par %s\n", token);  // cuando copilas y pones -D DEBUG, se copila con lo uqe esta entre #if
    #endif

    if (token != NULL) {
        strcpy(param, token);
    }else{
        fprintf(stderr, "Error: se esperaba un argumento para el comando %s. \n", operation);
        return 1;
    }
    return 0;  // Si todo sale bien, retorna 0.
}

