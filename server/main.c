#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "server.h"
#include "config.h"
#include "arguments.h"

int main(int argc, char const *argv[]){

    struct arguments args;

    if(parse_arguments(argc, argv, &args) != 0){
        return EXIT_FAILURE;
    }

    printf("Start server on %s:%d\n", args.address, args.port);

    int masterSocket = serverInit(args.address, args.port);
    if (masterSocker < 0){
        return EXIT_FAILURE;
    }

    //setup_signals();

    while(1){
        int slaveSocket = serverAccept(masterSocker);
        if (slaveSocket < 0){
            continue;
        }

        serverLoop(slaveSocket);

    }
/*
    int masterSocker, slaveSocket;
    struct sockaddr_in masterAddr, slaveAddr;
    socklen_t slaveAddrLen;
    char username[BUFSIZE];
    char password[BUFSIZE];
    char buffer[BUFSIZE];
    char command[BUFSIZE];
    int dataLen;
    
    masterSocker = socket(AF_INET, SOCK_STREAM, 0);
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Acepta conexiones de cualquier IP
    masterAddr.sin_port = htons(port);
    bind(masterSocker, (struct sockaddr *)&masterAddr, sizeof(masterAddr));
    listen(masterSocker, 5); // Escucha hasta 5 conexiones en cola
    while(1){
        slaveAddrLen = sizeof(slaveAddr);
        slaveSocket = accept(masterSocker, (struct sockaddr *)&slaveAddr, &slaveAddrLen);
        printf("%s \n", MSG_220);
        if(send(slaveSocket, MSG_220, sizeof(MSG_220)- 1, 0) != sizeof(MSG_220) - 1){
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo enviar el mensaje. \n");
            break;
        }

        if(recv_cmd(slaveSocket, command, username) != 0){
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo recibir el comando USER.\n");
            break;
        }; // Recibe el comando del cliente
        
        if (strcmp(command, "USER") != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: se esperaba el comando USER.\n");
            continue; // Si no se recibe el comando USER, cierra la conexión y espera una nueva
        }

        dataLen = snprintf(buffer, BUFSIZE, MSG_331, username);
        if(send(slaveSocket, buffer, dataLen, 0) < 0){
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo enviar el mensaje.\n");
            break;
        }

        if(recv_cmd(slaveSocket, command, password) != 0){
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo recibir el comando PASS.\n");
            break;
        } // Recibe el comando del cliente
        
        if(strcmp(command, "PASS") != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: se esperaba el comando PASS.\n");
            continue; // Si no se recibe el comando PASS, cierra la conexión y espera una nueva
        }
        // Si las credenciales son incorrectas, envia un mensaje de error y cierra la conexion
        if (!check_credentials(username, password)) {
            dataLen = snprintf(buffer, BUFSIZE, MSG_530);
            if (send(slaveSocket, buffer, dataLen, 0) < 0) {
                close(slaveSocket);
                fprintf(stderr, "Error: no se pudo enviar el mensaje.\n");
                break;
            }
            close(slaveSocket);
            continue; // Si las credenciales son incorrectas, cierra la conexión y espera una nueva
        }

        dataLen = snprintf(buffer, BUFSIZE, MSG_230, username);

        if(send(slaveSocket, buffer, dataLen, 0) < 0){
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo enviar el mensaje.\n");
            break;
        }

        while(1){
            if(recv_cmd(slaveSocket, command, buffer) != 0){
                close(slaveSocket);
                fprintf(stderr, "Error: no se pudo recibir el comando.\n");
                break;
            } // Recibe el comando del cliente
            
            if(strcmp(command, "QUIT") == 0){
                if(send(slaveSocket, MSG_221, sizeof(MSG_221) - 1, 0) < 0){
                    close(slaveSocket);
                    fprintf(stderr, "Error: no se pudo enviar el mensaje.\n");
                    break;
                }
                close(slaveSocket);
                break; // Si se recibe el comando QUIT, cierra la conexión y espera una nueva
            }
            if(strcmp(command, "SYST") == 0){
                if(send(slaveSocket, MSG_502, sizeof(MSG_502) - 1, 0) < 0){
                    close(slaveSocket);
                    fprintf(stderr, "Error: no se pudo enviar el mensaje SYST. \n");
                    break;
                }
                continue; // Si se recibe el comando SYST, envia la respuesta y espera un nuevo comando
            }

            // Aqui se pueden agregar mas comandos para manejar
            // Por ahora solo se maneja el comando QUIT
        }
    }
*/
}
