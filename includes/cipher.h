#ifndef CIPHER_H_
#define CIPHER_H_

#include "arguments.h"
#include "aes.h"

#define NB_BLOCK 1024
#define BUF_SIZE NB_BLOCK*BLOCKLEN

int read_file_input(struct operation *op, char *buffer);
int write_output(struct operation *op, char *buf, int byte_size);
uint8_t check_padding(char *buffer);
void HMAC(struct operation *op, char *Buffer);
void checkHMAC(struct operation *op);

#endif
