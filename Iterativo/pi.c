#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "pi.h"
#include "server.h"
#include "responses.h"
#include "utils.h"


static ftp_command_t ftp_commands[] = {
  { "USER", handle_USER },
  { "PASS", handle_PASS },
  { "QUIT", handle_QUIT },
  { "SYST", handle_SYST },
  { "TYPE", handle_TYPE },
  { "PORT", handle_PORT },
  { "RETR", handle_RETR },
  { "STOR", handle_STOR },
  { "NOOP", handle_NOOP },
  { NULL, NULL }
};

int welcome(ftp_session_t *sess) {

  // Send initial FTP welcome message
  if (safe_dprintf(sess->control_sock, MSG_220) != sizeof(MSG_220) - 1) {
    fprintf(stderr, "Send error\n");
    close_fd(sess->control_sock, "cliente socket");
    return -1;
  }

  return 0;
}

int get_exe_command(ftp_session_t *sess) {
  char buffer[BUFSIZE];

  // Recibe datos del cliente
  ssize_t len = recv(sess->control_sock, buffer, sizeof(buffer) - 1, 0);
  if (len < 0) {
    perror("Receive fail: ");
    close_fd(sess->control_sock, "cliente socket");
    return -1;
  }

  // La conexion se cerro inesperadamente. Cerramos el socket.
  if (len == 0) {
    sess->current_user[0] = '\0'; 
    close_fd(sess->control_sock, "client socket"); 
    sess->control_sock = -1;
    return -1;
  }

  buffer[len] = '\0';

  // printf("DEBUG: Received raw command: '%s'\n", buffer);

  // Strip CRLF
  char *cr = strchr(buffer, '\r');
  if (cr) *cr = '\0';
  char *lf = strchr(buffer, '\n');
  if (lf) *lf = '\0';

  // Separate command and argument
  char *arg = NULL;
  char *cmd = buffer;

  // printf("DEBUG: Received command: '%s' with args: '%s'\n", cmd, arg ? arg : "(null)");

  // Si el comando es nulo, retorna un error
  if (cmd[0] == '\0') {
    safe_dprintf(sess->control_sock, MSG_505);
    return 0;
  }

  // Si hay un espacio, separa el comando del argumento
  // Primero separar cmd y arg
  char *space = strchr(buffer, ' ');
  if (space) {
    *space = '\0';
    arg = space + 1;
    while (*arg == ' ') arg++;
  }

  //printf("DEBUG: Received command: '%s' with args: '%s'\n", cmd, arg ? arg : "(null)");

  // Busca el comando en la lista de comandos válidos
  ftp_command_t *entry = ftp_commands;
  while (entry->name) {       // Recoorre la lista de comandos
    
    if (strcasecmp(entry->name, cmd) == 0) {    // Si lo encuentra, llama al handler
      entry->handler(arg ? arg : "");
      return (sess->control_sock < 0) ? -1 : 0;
    }
    entry++;
  }

  // Si no se encuentra el comando, envía un mensaje de error de que no está implementado el comando dado.
  safe_dprintf(sess->control_sock, MSG_502);
  return 0;
}