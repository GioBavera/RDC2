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
  (void)args;
  (void)sess;

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  int h1, h2, h3, h4, p1, p2;
  if (sscanf(args, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  // Limpiar y configurar dirección IP y puerto para la conexión de datos
  memset(&sess->data_addr, 0, sizeof(sess->data_addr));
  sess->data_addr.sin_family = AF_INET;
  sess->data_addr.sin_port = htons(p1 * 256 + p2);

  char ip_str[INET_ADDRSTRLEN];
  snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", h1, h2, h3, h4);
  if (inet_pton(AF_INET, ip_str, &sess->data_addr.sin_addr) <= 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_200);
}

// Maneja el comando RETR (retrieve) para descargar un archivo.
void handle_RETR(const char *args) {
  ftp_session_t *sess = session_get();

  if (!sess->logged_in) {   // Session no iniciada
    safe_dprintf(sess->control_sock, MSG_530);
    return;
  }

  if (!args || strlen(args) == 0) { // Argumento vacío
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  // Abrir conexión de datos
  safe_dprintf(sess->control_sock, MSG_150); 

  // Intentar abrir el archivo para lectura
  FILE *fp = fopen(args, "rb");
  if (!fp) {
    perror("fopen");
    safe_dprintf(sess->control_sock, MSG_550, "No se pudo abrir el archivo");
    return;
  }

  int data_sock = socket(AF_INET, SOCK_STREAM, 0);
  // Verificar si se creó el socket de datos correctamente
  if (data_sock < 0) {
    perror("socket");
    fclose(fp);
    safe_dprintf(sess->control_sock, "No se pudo crear el socket de datos");
    return;
  }

  // Intentar conectar al socket de datos
  if (connect(data_sock, (struct sockaddr *)&sess->data_addr, sizeof(sess->data_addr)) < 0) {
    perror("connect");
    close(data_sock);
    fclose(fp);
    safe_dprintf(sess->control_sock, "No se pudo conectar el socket de datos");
    return;
  }

  char buffer[1024];
  size_t bytes_read;
  ssize_t bytes_written;
  int error = 0;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    bytes_written = write(data_sock, buffer, bytes_read);
    if (bytes_written < 0) {
      perror("write");
      error = 1;
      break;
    } else if ((size_t)bytes_written != bytes_read) {
      fprintf(stderr, "Error: no se escribieron todos los bytes\n");
      error = 1;
      break;
    }
  }

  if (ferror(fp)) {
    fprintf(stderr, "Error al leer el archivo\n");
    error = 1;
  }

  close(data_sock);
  fclose(fp);

  if (!error) {
    safe_dprintf(sess->control_sock, MSG_226); 
  } else {
    safe_dprintf(sess->control_sock, "Error durante la transferencia");
  }
}

// Maneja el comando STOR (store) para subir un archivo.
void handle_STOR(const char *args) {
  ftp_session_t *sess = session_get();


  if (!sess->logged_in) {
    safe_dprintf(sess->control_sock, MSG_530);
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_150); 

  FILE *fp = fopen(args, "wb");
  if (!fp) {
    perror("fopen");
    safe_dprintf(sess->control_sock, "No se pudo abrir el archivo para escritura");
    return;
  }

  int data_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (data_sock < 0) {
    perror("socket");
    fclose(fp);
    safe_dprintf(sess->control_sock, "No se pudo crear el socket de datos");
    return;
  }

  if (connect(data_sock, (struct sockaddr *)&sess->data_addr, sizeof(sess->data_addr)) < 0) {
    perror("connect");
    close(data_sock);
    fclose(fp);
    safe_dprintf(sess->control_sock, "No se pudo conectar el socket de datos");
    return;
  }

  char buffer[1024];
  ssize_t bytes;
  int error = 0;

  while ((bytes = read(data_sock, buffer, sizeof(buffer))) > 0) {
    if (fwrite(buffer, 1, bytes, fp) != (size_t)bytes) {
      perror("fwrite");
      error = 1;
      break;
    }
  }

  if (bytes < 0) {
    perror("read");
    error = 1;
  }

  if (ferror(fp)) {
    fprintf(stderr, "Error en el archivo luego del cierre.\n");
    error = 1;
  }

  fclose(fp);
  close(data_sock);

  if (!error) {
    safe_dprintf(sess->control_sock, MSG_226);
  } else {
    safe_dprintf(sess->control_sock, "No se pudo guardar el archivo");
  }
}

// Maneja el comando NOOP (no operation) que no realiza ninguna acción.
void handle_NOOP(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Responde con código 200 OK
  safe_dprintf(sess->control_sock, MSG_200);  // Usa el definido en responses.
}