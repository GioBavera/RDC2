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

int main(int argc, char **argv) {

    struct arguments args;
    // Valida que los argumentos se hayan inicializado correctamente.
    if(parse_arguments(argc, argv, &args) != 0){
        return EXIT_FAILURE;
    }

    printf("Start server on %s:%d\n", args.address, args.port);

    // Con los argumentos validados, inicializa el servidor.
    int masterSocket = serverInit(args.address, args.port);
    if (masterSocket < 0){
        return EXIT_FAILURE;
    }

    // Configura las señales para manejar interrupciones y otras condiciones.
    setup_signals();

    // Inicia el bucle principal del servidor, aceptando conexiones entrantes.
    printf("Server is running...\n");
    // Bucle infinito para aceptar conexiones entrantes. Si hay exito, devuelve el socket del cliente y entra en loop. 
    // Si falla, espera un nuevo cliente.
    while(1){
        struct sockaddr_in client_addr;
        int new_socket = serverAccept(masterSocket, &client_addr);
        if (new_socket < 0){
            continue;
        }

        // Imprime la dirección IP y el puerto del cliente que se ha conectado.
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection from %s:%d accepted\n", client_ip, ntohs(client_addr.sin_port));

        serverLoop(new_socket);

        printf("Connection from %s:%d closed\n", client_ip, ntohs(client_addr.sin_port));
    }

    // Cierra el socket maestro al finalizar.
    close(masterSocket);
    return EXIT_SUCCESS;
}