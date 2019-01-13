/**
 * Projet BCS - Master 2 SSI - Istic (Univ. Rennes1)
 *
 * Andriamilanto Tompoariniaina
 * Genevey-metat Christophe
 */
#include "arguments.h"


/**
 * Process the password to give a key instead, uses a simple SHA3-256
 */
void password_to_key(struct operation *op) {
	FIPS202_SHA3_256((unsigned char *)op->password, (unsigned int)sizeof(op->password), (unsigned char *)op->key);
}


/**
 * Function to generate a IV(nonce)
 */
void generate_IV(struct operation *op) {
	// Generate a random nonce
	op->iv = MRG32k3a();
}


/**
 * Generate a counter using an IV and an ID
 */
void get_nonce_counter(struct operation *op, char *counter, double id) {
	memcpy(counter, &op->iv, sizeof(double));
	memcpy(counter+8, &id, sizeof(double));
}


// Manage the IV
// Encrypt => Save it at the beginning of the output file
// Decrypt => Read it from the beginning of the input file
void manage_IV(struct operation *op) {

	// If an encrypt
	if (op->type == encryption) {

		// Generate a singled IV
		generate_IV(op);

		// Write it into the output file (8 * 1 Byte)
		if (fwrite(&op->iv, 8, 1, op->f_output) == 0)
			printf("%s", "ERROR: I/O Err writting IV into output file");

	// If a decrypt
	} else {

		// Read the IV from the file
		if (fread(&op->iv, 8, 1, op->f_input) == 0)
			printf("%s", "ERROR: I/O Err reading IV from input file");
	}
}


/**
 * Simple handler to manage the parameters passed
 * \param argc The number of arguments given
 * \param **argv A pointer to the arguments given
 * \param *op A pointer to the operation structure in which we will read/write
 */
void arguments_handler(int argc, char **argv, struct operation *op) {

	// If not enough arguments
	if (argc != NB_ARGS) {
		printf("ERROR: Wrong syntax, the correct one is the following:\n");
		printf("%s -c/d [file_in] -o [file_out] -k [password]\n", argv[0]);
		exit(-1);
	}

	// The filenames
	char input[PATH_MAX], output[PATH_MAX];

	//Structure to have the size file
	struct stat file;

	// For each couple of arguments
	unsigned int i;
	for (i = 1; i < NB_ARGS; i += 2) {

		// If the input file for encryption
		if (strncmp("-c", argv[i], 2) == 0) {
			strncpy(input, argv[i+1], PATH_MAX);
			lstat(argv[i+1],&file);
			op->size_file = file.st_size;
			op->type = encryption;
		}

		// If the input file for decryption
		else if (strncmp("-d", argv[i], 2) == 0) {
			strncpy(input, argv[i+1], PATH_MAX);
			lstat(argv[i+1],&file);
			op->size_file = file.st_size;
			op->type = decryption;
		}

		// If the output file
		else if (strncmp("-o", argv[i], 2) == 0)
			strncpy(output, argv[i+1], PATH_MAX);

		// If the password file
		else if (strncmp("-k", argv[i], 2) == 0)
			strncpy(op->password, argv[i+1], PATH_MAX);
	}

	// Open the input file
	op->f_input = fopen(input, "r");
	if (op->f_input == NULL) {
		printf("ERROR: Can't open file: %s", input);
		exit(-1);
	}

	// Open the output file
	op->f_output = fopen(output, "w+");
	if (op->f_output == NULL) {

		// Don't forget to close input file
		fclose(op->f_input);

		printf("ERROR: Can't open file: %s", output);
		exit(-1);
	}

	// Prepare the key (get the SHA3_256 hash of it)
	FIPS202_SHA3_256((unsigned char *)op->password, (unsigned int)sizeof(op->password), (unsigned char *)&(op->key));

	// Manage the IV
	// Encrypt => Save it at the beginning of the output file
	// Decrypt => Read it from the beginning of the input file
	manage_IV(op);

	// Display each input parameters
	// printf(
	// 	"Input file is %s\nOuput file is %s\nPassword is %s\n",
	// 	input, output, op->password
	// );
}
