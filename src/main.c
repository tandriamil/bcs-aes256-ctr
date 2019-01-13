/**
 * Projet BCS - Master 2 SSI - Istic (Univ. Rennes1)
 *
 * Andriamilanto Tompoariniaina
 * Genevey-metat Christophe
 */
#include "mrg32k3a.h"
#include "arguments.h"
#include "aes.h"
#include "cipher.h"


/**
 * Main function that is launched
 */
int main(int argc, char **argv) {

	// The local variables that are used here
	struct operation op;
	uint8_t padding_remove;
	int i, nb_blocks_read, nb_bytes_read, loop_id = 0;
	char counter[16];
	double unique_id;
	off_t rest;

	// We just pass the arguments to the handler
	arguments_handler(argc, argv, &op);

	// Prepare the two buffers, entry and output
	char buf_in[BUF_SIZE+BLOCKLEN], buf_out[BUF_SIZE+BLOCKLEN];

	// Get the bytes left to read
	rest = op.size_file;

	// If decryption
	if (op.type == decryption) {

		// Check that the HMAC is correct
		checkHMAC(&op);

		// Remove the size of the IV situated at the beginning  (64b = 8B)
		rest -= 8;

		// Put the pointer behind the IV
		fseek(op.f_input, 8, SEEK_SET);
	}

	// Read the Bytes until there is no more
	do {

		// Fill the first buffer
		memset(buf_in, 0, BUF_SIZE+BLOCKLEN);
		memset(buf_out, 0, BUF_SIZE+BLOCKLEN);

		// Get the number of bytes read and so the left bytes to read
		nb_bytes_read = read_file_input(&op, buf_in);
		// printf("nb bytes read = %d\n", nb_bytes_read);

		//Get the left bytes to read
		rest -= nb_bytes_read;
		// printf("nb octets restant : %ld\n", rest);

		// Get the number of blocks of 16B to read
		nb_blocks_read = ceil((double)nb_bytes_read / BLOCKLEN);
		// printf("nb blocks read = %d\n", nb_blocks_read);

		// If not the last block of 1024*16B
		if (rest > 0) {

			// Process the cipher on it
			for (i = 0; i < nb_blocks_read; ++i) {

				// Get the message/cipher of each block
				unique_id = i + (loop_id * NB_BLOCK);
				get_nonce_counter(&op, counter, unique_id);
				ctr(buf_in+i*BLOCKLEN, buf_out+i*BLOCKLEN, op.key, counter);
			}

			// Write the output buffer
			write_output(&op, buf_out, BLOCKLEN * nb_blocks_read);

		// If last block
		} else {

			// Remove the 2 blocks of 16B composing the hash if decryption
			if (op.type == decryption) nb_blocks_read -= 2;

			// Process the cipher on it
			for (i = 0; i < nb_blocks_read-1; ++i) {

				// Get the cipher on each block
				unique_id = i + (loop_id * NB_BLOCK);
				get_nonce_counter(&op, counter, unique_id);
				ctr(buf_in+i*BLOCKLEN, buf_out+i*BLOCKLEN, op.key, counter);
			}

			// Process the LAST block (maybe need padding!)
			unique_id = i + (loop_id * NB_BLOCK);
			get_nonce_counter(&op, counter, unique_id);

			// Here, padding process if encryption
			if (op.type == encryption) {

				// Get the size of the padding to do
				uint8_t padding = (nb_blocks_read * BLOCKLEN) - nb_bytes_read;

				// If the last buffer is complete, we add a buffer of padding
				if (padding == 0) {

					// The additionnal block!
					int additional_id = i + 1;
					double additional_unique_id = additional_id + (loop_id * NB_BLOCK);
					char additional_counter[16];
					get_nonce_counter(&op, additional_counter, additional_unique_id);
					memset(buf_in+(additional_id*BLOCKLEN), 0x0, 16);

					// Cipher the block
					ctr(buf_in+additional_id*BLOCKLEN, buf_out+additional_id*BLOCKLEN, op.key, additional_counter);

					// Adding this part to be stored into the file!
					++nb_blocks_read;
				}

				// The last buffer is not complete, we add padding into the last buffer
				else {
					memset(buf_in+(i*BLOCKLEN) + (BLOCKLEN - padding), padding, padding);
				}
			}

			// (Un)Cipher the block
			ctr(buf_in+i*BLOCKLEN, buf_out+i*BLOCKLEN, op.key, counter);

			// If decryption, put the number of bytes to remove
			padding_remove = 0;
			if (op.type == decryption)
				padding_remove = check_padding(buf_out+i*BLOCKLEN);

			// Write the output buffer
			write_output(&op, buf_out, BLOCKLEN * nb_blocks_read - padding_remove);
		}

		// Increment the loop id
		loop_id++;

	} while(rest > 0);


	// Compute the HMAC for the encryption
	if (op.type == encryption) {

		// Put the fp back to the beginning
		rewind(op.f_output);

		// Get the HMAC of the output file
		char buffer_hmac[32];
		HMAC(&op, buffer_hmac);

		// Write the value of the HMAC at the end of it
		write_output(&op, buffer_hmac, 32);
	}

	// Close files then exit with code = 0 = OK
	fclose(op.f_input);
	fclose(op.f_output);
	return 0;
}
