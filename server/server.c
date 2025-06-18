#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "dtp.h"
#include "server.h"
#include "pi.h"

int serverInit(const char *ip, int port){

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0){
        perror("Error socket creation");
        return -1;
    }

    const int opt = 1;
    if (setSockopt(serverSocket, SOL_SOCKET, SO_REUSEADOR, &opt, sizeof(opt)) < 0){
        perror("Error setting SO_REUSEADOR");
        close(serverSocket);
        return -1;
    }

    struct sockaddr_in masterAddr;
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &masterAddr.sin_addr) < 0){
        fprintf(stderr, "Invalid IP address %s\n", ip);
        close(serverSocket);
        return -1;
    }

    if (bind(masterSocker, (struct sockaddr *)&masterAddr, sizeof(masterAddr)) < 0){
            perror("Bind failed.");
            close(masterSocker);
            return -1;
    }

    if(listen(masterSocker, SOMAXCON) < 0){
        perror("Listen failed.");
        close(masterSocker);
        return -1;
    }

    return serverSocket;
}

int serverAccept(int socket){

    struct sockaddr slaveAddr;
    socketlen_t slaveAddrLen = sizeof(slaveAddr);
    slaveSocket = accept(socket, (struct sockaddr *)&slaveAddr, &slaveAddrLen);
    if (slaveSocket < 0){
        perror("Accept failed.");
        return -1;
    }

    char ipStr[INET_ADDRLEN];
    inet_ntop(AF_INET, &slaveAddr.sin_addr, buffer, sizeof(ipStr))
    fprintf(stderr, "Connection from %s:%d/n", ipStr, ntohs(slaveAddr.sin_port));

    return slaveSocket;
}

void serverLoop(int socket){
    sessionInit(socket);

    if(wwelcome(current_sess) < 0){
        return;
    }

    while(1){
        if (get_exe_command(current_sess) < 0){
            break;
        }
    }

    sessionClose();
}
/*

    socklen_t slaveAddrLen;
    char username[BUFSIZE];
    char password[BUFSIZE];
    char buffer[BUFSIZE];
    char command[BUFSIZE];
    int dataLen;
    
    while(1){
        slaveAddrLen = sizeof(slaveAddr);
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

    close(masterSocker);

    return 0;
}
