/**
 * @file hash-file.c
 * @brief Hash program file containing main function
 * @author Marek Felsoci
 **/
 
#include "../hash.h"
#include "hash.c"

/**
 * @brief Main function. Computes SHA256 hashes of the file passed as the first and unique argument
 **/
int main(int argc, char ** argv) 
{
	if(argc != 2) 
	{
		fprintf(stderr, "Options or arguments mismatch! Please use %s as follows\n%s <file-path>\nCheck your request and try again!\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	
	char * filename = argv[1];
	
	unsigned char * fhash = hash_entire_file(filename);
	unsigned char ** chunks = chunk_and_hash(filename);
	
	printh(fhash);
	printc(chunks);

	free(fhash);	
	free_chunk_hash_table(chunks);
	
	return 0;
}
