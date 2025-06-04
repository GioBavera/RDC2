#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "serverdtp.h"
#include "serverausftp.h"

#define VERSION "1.0"
#define PTODEFAULT 21

int main(int argc, char const *argv[]){
    
    int port;

    if (argc > 2){
        fprintf(stderr, "Error: mal ingreso de argumentos\n");
        return 1;
    }

    if (argc = 2){
        port = atoi(argv[1]);
    }else{
        port = PTODEFAULT; // Puerto default para FTP
    }

    if(port == 0){
        fprintf(stderr, "Error: puerto invalido\n");
        return 1;
    }

    printf("%d\n", port);

    printf("Devuelve: %d\n", check_credentials("pepe", "pepe"));

    int masterSocker, slaveSocket;
    struct sockaddr_in masterAddr, slaveAddr;
    socklen_t slaveAddrLen;
    
    masterSocker = socket(AF_INET, SOCK_STREAM, 0);
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_addr.s_addr = INADDR_ANY; // Acepta conexiones de cualquier IP
    masterAddr.sin_port = htons(port);

    bind(masterSocker, (struct sockaddr *)&masterAddr, sizeof(masterAddr));
    listen(masterSocker, 5); // Escucha hasta 5 conexiones en cola
    while(1){
        slaveAddrLen = sizeof(slaveAddr);
        slaveSocket = accept(masterSocker, (struct sockaddr *)&slaveAddr, &slaveAddrLen);
        send(slaveSocket, "220 1", sizeof("220 1"), 0); // Mensaje de bienvenida
        // Al mandarle esto, el cliente deberia devolver algo.
        printf("Funciono. \n");
    }

    close(masterSocker);

    return 0;
}
