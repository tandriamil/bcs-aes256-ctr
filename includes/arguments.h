#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mrg32k3a.h"
#include "keccak.h"

#define NB_ARGS 7

enum op_type { encryption, decryption };
struct operation {
	FILE *f_input;
	FILE *f_output;
	off_t size_file;
	char password[PATH_MAX];
	enum op_type type;

	double iv;
	char key[32];
};

void arguments_handler(int argc, char **argv, struct operation *op);
void get_nonce_counter(struct operation *op, char *counter, double id);

#endif
