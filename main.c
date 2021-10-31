#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define OP_LEN 10

// Define operations
static const char operations[][OP_LEN] = {
	"zero",
	"zero_rm",
	"one",
	"one_rm",
	"random",
	"random_rm"
};
// Number of operations
static const unsigned int num_ops = sizeof(operations) / (sizeof(char) * OP_LEN);

// Function constructors
void print_help(char *prog_name);
unsigned int check_operation(char *operation);
unsigned int file_size(char *file);
unsigned int perform_operation(char *operation, char *file);

int main(int argc, char *argv[]) {
	//Print Error on too many arguments
	if (argc < 3) {
		printf("\aNeed to specify file and operation\n");
		print_help(argv[0]);
		return -1;
	}

	//Print error on too many arguments
	else if (argc > 3) {
		printf("\aToo many arguments\n");
		print_help(argv[0]);
		return -1;
	}

	// Check operation
	if (!check_operation(argv[1])) {
		printf("\aInvalid operation\n");
		print_help(argv[0]);
		return -1;
	}

	// Check file
	if (access(argv[2], W_OK)) {
		printf("\aUnable to write to file\n");
		printf("Make %s exists and that you have write permissions on the file", argv[2]);
		return -1;
	}

	// Perform Operation and Exit
	return perform_operation(argv[1], argv[2]);
}

// Print syntax and available operations
void print_help(char *prog_name) {
	//Print syntax
	printf("\nSyntax: \n");
	printf("%s operation file\n", prog_name);

	//Print operations
	printf("\nAvailable Operations:\n");
	for (unsigned int i = 0; i < num_ops; i++) {
		printf("* %s\n", operations[i]);
	}
}

// Check existence of an operation
unsigned int check_operation(char *operation) {
	for (unsigned int i = 0; i < num_ops; i++) {
		if (!strcmp(operation, operations[i])) {
			return 1;
		}
	}

	return 0;
}

// Perform operation
unsigned int perform_operation(char *operation, char *file) {
	unsigned int size = file_size(file);
	unsigned int written = 0;
	unsigned char random = 0;
	unsigned char data = 0;

	int fd = open(file, O_WRONLY|O_TRUNC);

	//Zeros
	if (!strncmp(operation, "zero", 4)) {
		data = 0b00000000;
	}

	//Ones
	else if (!strncmp(operation, "one", 3)) {
		data = 0b11111111;
	}

	//Random
	else if (!strncmp(operation, "random", 6)) {
		srand(time(NULL));
		random = 1;
	}

	//Write data
	while (written < size) {
		//Generate random 1 byte (8 bit) unsigned char if using random
		if (random) {
			data = (unsigned char) rand();
		}
		written += write(fd, &data, sizeof(data));
		lseek(fd, written, SEEK_SET);
	}

	char *rm = strchr(operation, '_');

	//Delete file if requested
	if (!strcmp(rm, "_rm")) {
		if (remove(file)) {
			printf("Failed to delete file\n");
			return -1;
		}
	}

	return 0;
}

// Check size of file
unsigned int file_size(char *file) {
	struct stat st;
	stat(file, &st);
	return st.st_size;
}
