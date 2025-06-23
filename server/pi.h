#pragma once

#include <stddef.h>
#include "session.h"
#include "responses.h"

#define MAX_CMD_LEN 16
#define MAX_PARAM_LEN 256

int welcome(ftp_session_t *sess);
int get_exe_command(ftp_session_t *sess);
int recv_cmd(int sd, char *operation, char *param);

typedef struct {
  const char *name;
  void (*handler)(const char *args);
} ftp_command_t;

extern const char *valid_commands[];
extern const int arg_commands[];

void handle_USER(const char *args);
void handle_PASS(const char *args);
void handle_QUIT(const char *args);
void handle_SYST(const char *args);
void handle_TYPE(const char *args);
void handle_PORT(const char *args);
void handle_RETR(const char *args);
void handle_STOR(const char *args);
void handle_NOOP(const char *args);