#ifndef ENCLAVE_MANAGEMENT_H
#define ENCLAVE_MANAGEMENT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

# define TOKEN_FILENAME   "trusted.token"
# define ENCLAVE_FILENAME "trusted.signed.so"

int initialize_enclave();
void destroy_enlave();

#endif