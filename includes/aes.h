#ifndef AES_H_
#define AES_H_

#include <stdint.h>

// EDIT: The parameters below have been modified to be used as AES256
// The number of columns comprising a state in AES.
// This is a constant in AES. Value=4
#define Nb 4

// Block length in bytes 128b (16B)
#define BLOCKLEN 16

// The number of 32 bit words in a key.
#define Nk 8

// The number of rounds in AES Cipher. For AES-256 it is 14
#define Nr 14

// jcallan@github points out that declaring Multiply as a function
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES128-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
  #define MULTIPLY_AS_A_FUNCTION 0
#endif

void ctr(char *input, char *output, char *key, char *counter);

#endif
