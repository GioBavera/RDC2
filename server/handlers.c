// handlers.c

#include "responses.h"
#include "pi.h"
#include "dtp.h"
#include "session.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


void handle_USER(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Syntax error in parameters
    return;
  }

  strncpy(sess->current_user, args, sizeof(sess->current_user) - 1);
  sess->current_user[sizeof(sess->current_user) - 1] = '\0';
  safe_dprintf(sess->control_sock, MSG_331); // Username okay, need password
}

void handle_PASS(const char *args) {

  ftp_session_t *sess = session_get();

  if (sess->current_user[0] == '\0') {
    safe_dprintf(sess->control_sock, MSG_503); // Bad sequence of commands
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Syntax error in parameters
    return;
  }

  if (check_credentials(sess->current_user, (char *)args) == 0) {
    sess->logged_in = 1;
    safe_dprintf(sess->control_sock, MSG_230); // User logged in
  } else {
    safe_dprintf(sess->control_sock, MSG_530); // Not logged in
    sess->current_user[0] = '\0'; // Reset user on failed login
    sess->logged_in = 0;
  }
}

void handle_QUIT(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused

  safe_dprintf(sess->control_sock, MSG_221); // 221 Goodbye.
  sess->current_user[0] = '\0'; // Close session
  close_fd(sess->control_sock, "client socket"); // Close socket
  sess->control_sock = -1;
}

void handle_SYST(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused

  safe_dprintf(sess->control_sock, MSG_215); // 215 <system type>
}

// Maneja el comando TYPE para establecer el tipo de transferencia.
// 'A' para ASCII, 'I' para binario. Por defecto es 'I'.
void handle_TYPE(const char *args) {
    ftp_session_t *sess = session_get();

    // Error de escritura
    if (!args || strlen(args) == 0) {
        safe_dprintf(sess->control_sock, MSG_501); 
        return;
    }

    if (strcmp(args, "A") == 0 || strcmp(args, "a") == 0) {
        sess->transfer_type = 'A';
        safe_dprintf(sess->control_sock, MSG_555);
    } else if (strcmp(args, "I") == 0 || strcmp(args, "i") == 0) {
        sess->transfer_type = 'I';
        safe_dprintf(sess->control_sock, MSG_556);
    } else {
        safe_dprintf(sess->control_sock, MSG_557);
    }
  }

// Cambia la la dirección y puerto de datos para la conexión activa.
void handle_PORT(const char *args) {
    ftp_session_t *sess = session_get();

    // Error de escritura
    if (!args || strlen(args) == 0) {
        safe_dprintf(sess->control_sock, MSG_501);  // Syntax error
        return;
    }

    // Se espera un formato de 6 números separados por comas: h1,h2,h3,h4,p1,p2
    // No se agregaron todos los argumentos.
    unsigned int h1, h2, h3, h4, p1, p2;
    if (sscanf(args, "%u,%u,%u,%u,%u,%u", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
        safe_dprintf(sess->control_sock, MSG_501);  
        return;
    }

    if (h1 > 255 || h2 > 255 || h3 > 255 || h4 > 255 || p1 > 255 || p2 > 255) {
        safe_dprintf(sess->control_sock, MSG_501);  // Syntax error
        return;
    }

    memset(&sess->data_addr, 0, sizeof(sess->data_addr));
    sess->data_addr.sin_family = AF_INET;
    sess->data_addr.sin_addr.s_addr = htonl((h1 << 24) | (h2 << 16) | (h3 << 8) | h4);
    sess->data_addr.sin_port = htons((p1 << 8) | p2);

    safe_dprintf(sess->control_sock, MSG_200);  // OK
}

// Maneja el comando RETR (retrieve) para descargar un archivo.
void handle_RETR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused
}

// Maneja el comando STOR (store) para subir un archivo.
void handle_STOR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused
}

// Maneja el comando NOOP (no operation) que no realiza ninguna acción.
void handle_NOOP(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Responde con código 200 OK
  safe_dprintf(sess->control_sock, MSG_200);  // Usa el definido en responses.
}