#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "server.h"
#include "pi.h"
#include "session.h"
#include "config.h"
#include "utils.h"

extern int server_socket;

// Crea un socket de servidor y lo configura para escuchar en la dirección IP y puerto especificados.
int serverInit(const char *ip, int port){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0){
        perror("Error socket creation");
        return -1;
    }

    const int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Error setting SO_REUSEADDR");
        close(serverSocket);
        return -1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0){
        fprintf(stderr, "Invalid IP address %s\n", ip);
        close(serverSocket);
        return -1;
    }

    // Vincula el socket a la dirección y puerto especificados.
    // Si falla, imprime un mensaje de error y cierra el socket.
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
            perror("Bind failed.");
            close(serverSocket);
            return -1;
    }

    // Configura el socket para escuchar conexiones entrantes.
    if(listen(serverSocket, SOMAXCONN) < 0){
        perror("Listen failed.");
        close(serverSocket);
        return -1;
    }

    server_socket = serverSocket; // Guarda el socket del servidor en una variable global
    return serverSocket;
}


int serverAccept(int listen_fd, struct sockaddr_in *client_addr) {

  socklen_t addrlen = sizeof(*client_addr);
  int new_socket = accept(listen_fd, (struct sockaddr *)client_addr, &addrlen);

  // EINTR for avoid errors by signal reentry
  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
  if (new_socket < 0 && errno != EINTR) {
    fprintf(stderr, "Accept failed: ");
    perror(NULL);
    return -1;
  }

  return new_socket;
}

// Desde el main se envia aca, donde primero se inicializa la sesion con el socket del cliente.
void serverLoop(int socket){
    // Inicializa la sesión FTP con el socket del cliente.
    session_init(socket);

    // Envía mensaje de bienvenida al cliente.
    if(welcome(current_sess) < 0){
        return;
    }

    // Bucle principal para recibir y ejecutar comandos FTP.
    while(1){
        if (get_exe_command(current_sess) < 0){
            break;
        }
    }

    // Limpia la sesión y cierra el socket del cliente al finalizar.
    session_cleanup();
    close(socket);
}
