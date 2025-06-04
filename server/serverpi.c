#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serverpi.h"
#include "serverausftp.h"

// LIBRERIAS DE SOCKET
#include <netinet/in.h>
#include <sys/socket.h>

// DEFINICIONES
#define MSG_220 "220 srvFtp version 1.0\r\n"
#define MSG_331 "331 Password required for %s\r\n"
#define MSG_230 "230 User %s logged in\r\n"
#define MSG_530 "530 Login incorrect\r\n"
#define MSG_221 "221 Goodbye\r\n"
#define MSG_550 "550 %s: no such file or directory\r\n"
#define MSG_299 "299 File %s size %ld bytes\r\n"
#define MSG_226 "226 Transfer complete\r\n"

/*
    * Este codigo recepciona un comando (recive) desde el socket descriptor sd. 
    * recv es el que hace efectivamente la recepcion de datos. Llama definida en syssocket. 
    * Si esto falla, hacemos un return 1.
*/
int recv_cmd(int sd, char *operation, char *param) {    // sd = socket descriptor, es local.
    char buffer[BUFSIZE];
    char *token;

    if (recv(sd, buffer, BUFSIZE, 0) < 0) {
        fprintf(stderr, "Error receiving data\n");
        return 1;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    token = strtok(buffer, " ");
    // strtok tiene un comportamiento inicial pero en adelante actua de forma distinta.

    if (token == NULL || strlen(token) < 4) {
        fprintf(stderr, "Comando FTP no valido\n");
        return 1;
    } else {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        // Aca le indica que siga trabajando con el mismo buffer. Si le paso uno nuevo, da otro token.
        // Utiliza static internamente para saber como actua.
        #if DEBUG 
        printf("par %s\n", token);  // cuando copilas y pones -D DEBUG, se copila con lo uqe esta entre #if
        #endif
        if (token != NULL) strcpy(param, token);
    }
}

