#pragma once

#include <stdbool.h>
#define PWDFILE "/etc/ausftp/ftpusers"

int check_credentials(char *user, char *pass);