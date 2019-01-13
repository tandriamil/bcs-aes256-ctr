/**
 * Projet BCS - Master 2 SSI - Istic (Univ. Rennes1)
 *
 * Andriamilanto Tompoariniaina
 * Genevey-metat Christophe
 */
#include "cipher.h"


/**
 * Function to read 1024 byte of data and put to buffer
 */
int read_file_input(struct operation *op, char *buffer) {
	return fread(buffer, 1, NB_BLOCK*BLOCKLEN, op->f_input);
}


/**
 * Function to write the output buffer into the output file
 */
int write_output(struct operation *op, char *buf, int byte_size) {
	return fwrite(buf, 1, byte_size, op->f_output);
}


/**
 * Function to know the type of padding at the end of the buffer
 */
uint8_t check_padding(char *buffer) {

	// Pointer to then end of the buffer
	// CARE! buffer + BLOCKLEN goes to the NEXT buffer
	char *end = buffer + BLOCKLEN - 1;

	// Loval variables
	uint8_t bytes_to_remove = 0, pad = *end;

	// Check that the other padding bytes are equal to this one
	if (pad > 0) bytes_to_remove = pad;

	// If a full block of padding
	else if (pad == 0) bytes_to_remove = BLOCKLEN;

	// Unknown case
	else printf("WARNING: Incorrect value of padding byte: %d\n", pad);

	// Get the value of the last byte (=> The number of padding byte)
	return bytes_to_remove;
}


/**
 * Function to evaluate HMAC
 */
void HMAC(struct operation *op, char *Buffer) {

	// Constants and intermediate values
	char opad[32], ipad[32], KxorOpad[32], KxorIpad[32], second_part[32], concat[64];
	int i, size_fread;

	// Set the two constants
	memset(opad, 0x5c, 32);
	memset(ipad, 0x36, 32);

	// Compute the size of output file (size with padding + 8B for IV)
	if(op->type == encryption)
		size_fread = ceil((double)op->size_file/BLOCKLEN) * BLOCKLEN + 8;
	else
		size_fread = op->size_file;

	// Size of the big buffer
	int taille_big_buffer = 32;  // Size of K_xor_Ipad
	if (op->type == encryption)
		taille_big_buffer += size_fread;
	else
		taille_big_buffer += size_fread - 32;  // Remove the size of the stored HMAC

	// Allocate the big buffer
	char *big_buffer = (char *)malloc(taille_big_buffer);
	if (big_buffer == NULL) {
		printf("Can't allocate %d bytes for the big buffer\n", taille_big_buffer);
		fclose(op->f_input);
		fclose(op->f_output);
		exit(-1);
	}

	// Compute the xor between the key and opad
	for (i = 0; i < 32; ++i)
		KxorOpad[i] = op->key[i] ^ opad[i];

	// Compute the xor between the key and ipad
	for (i = 0; i < 32; ++i)
		KxorIpad[i] = op->key[i] ^ ipad[i];

	// Prepare the big buffer
	memcpy(big_buffer, KxorIpad, 32);

	// We refresh the pointer in both cases
	if (op->type == encryption) {
		rewind(op->f_output);

		// Put the message into the big buffer
		fread(big_buffer+32, 1, size_fread, op->f_output);

	// Decryption
	} else {
		rewind(op->f_input);

		// If we decrypt we remove the HMAC in the file
		fread(big_buffer+32, 1, size_fread-32, op->f_input);
	}

	// Do the hash H((K xor ipad) || m)
	FIPS202_SHA3_256((unsigned char *)big_buffer, taille_big_buffer, (unsigned char *)&second_part);

	// Put the final concatenation
	memcpy(concat, KxorOpad, 32);
	memcpy(concat+32, second_part, 32);

	// Do the final hash
	FIPS202_SHA3_256((unsigned char *)concat, 64, (unsigned char *)Buffer);

	// Free the big buffer
	free(big_buffer);
}


/**
 * Function to check HMAC
 */
void checkHMAC(struct operation *op) {

	// Local variables
	char hmac1[32], hmac2[32], file_input[op->size_file-32];

	// Rewind the input file
	rewind(op->f_input);

	// Read the file cryto
	fread(file_input, 1, op->size_file-32, op->f_input);

	// Read the hmac of file crypt
	fread(hmac1, 1, 32, op->f_input);

	// Compute the hmac of file crypto
	HMAC(op, hmac2);

	// Verdict of the hashmac
	if (memcmp(hmac1, hmac2, 32)) {
		printf("ERROR: The HMAC is incorrect\n");
		fclose(op->f_input);
		fclose(op->f_output);
		exit(-1);
	}

	// Don't forget to rewing the input file
	rewind(op->f_input);
}
