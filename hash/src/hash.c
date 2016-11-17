/**
 * @file hash.c
 * @brief Hash functions source file
 * @author Marek Felsoci
 **/
 
#include "../hash.h"

/**
 * @brief This function raise error message regarding to the last executed system call and exits the program.
 * @param error_message : user defined error message to be printed on stderr
 * @returns void
 **/
void die(char * error_message)
{
	perror(error_message);
	
	exit(EXIT_FAILURE);
}

/**
 * @brief Creates SHA256 hash of an entire file
 * @param filename : string containing path to the file to be hashed
 * @returns unsigned character string containing SHA256 hash
 **/
unsigned char * hash_entire_file(char * filename)
{
	FILE * ftoh = fopen(filename, "r");
	
	if(ftoh == NULL) 
		die("Unable to access file to hash");
	
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	
	void * data = (void*) malloc(BUFFER_SIZE * sizeof(void));
	unsigned char * hash = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
	
	while(fread(data, 1, BUFFER_SIZE, ftoh) > 0)
	{
		SHA256_Update(&ctx, data, BUFFER_SIZE);
	} 
	
	SHA256_Final(hash, &ctx);
	
	free(data);
	
	fclose(ftoh);
	
	return hash;
}

/**
 * @brief Compute how many chunks the file will be chunked in
 * @param filename : string containing path to the file to compute chunks of
 * @returns integer containing chunk count this file needs to be chunked in
 **/
int get_chunk_count(char * filename)
{
	FILE * ftoc = fopen(filename, "r");
	
	if(ftoc == NULL) 
		die("Unable to access file to count chunk number");
	
	struct stat st_buf;
	
	if(stat(filename, &st_buf) == -1)
		die("Error retrieving the size of the file to chunk");
	
	int whole_file_size = (int) st_buf.st_size;
	
	int result = -1;
	
	if(whole_file_size % CHUNK_SIZE_IN_BYTES == 0)
	{
		result = whole_file_size / CHUNK_SIZE_IN_BYTES;
	}
	else
	{
		result = (whole_file_size / CHUNK_SIZE_IN_BYTES) + 1;
	}
	
	fclose(ftoc);
	
	return result;
}

/**
 * @brief Generate a hash table containing SHA256 hashes of all file chunks
 * @param filename : string containing path to the file the chunk hashes will be computed
 * @returns dynamic array of unsigned character strings containing SHA256 chunk hashes 
 **/
unsigned char ** chunk_and_hash(char * filename)
{
	FILE * ftoch = fopen(filename, "r");
	
	if(ftoch == NULL)
		die("Unable to access the file to generate chunk hash table");
	
	int i;
	int read_bytes_count;
	int chunk_count = get_chunk_count(filename);
	
	if(chunk_count == 1)
	{
		fclose(ftoch);
		
		unsigned char ** hash_table = (unsigned char**) malloc((chunk_count+1) * sizeof(unsigned char*));
		hash_table[0] = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
		hash_table[0] = hash_entire_file(filename);
		hash_table[1] = NULL;
		
		return hash_table;
	}
	
	SHA256_CTX * ctx = (SHA256_CTX*) malloc(chunk_count * sizeof(SHA256_CTX));
	
	void * data = (void*) malloc(BUFFER_SIZE * sizeof(void));
	unsigned char ** hash_table = (unsigned char**) malloc((chunk_count+1) * sizeof(unsigned char*));
	
	for(i = 0;i < chunk_count;i++)
		hash_table[i] = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
	
	for(i = 0;i < chunk_count;i++)
	{
		SHA256_Init(&ctx[i]);
		do
		{
			read_bytes_count = fread(data, 1, BUFFER_SIZE, ftoch);
			SHA256_Update(&ctx[i], data, BUFFER_SIZE);
		}
		while(read_bytes_count == CHUNK_SIZE_IN_BYTES );
		SHA256_Final(hash_table[i], &ctx[i]);
	}
	
	free(ctx);
	free(data);
	fclose(ftoch);
	
	hash_table[chunk_count] = NULL;
	
	return hash_table;
}

/**
 * @brief Frees the memory allocated for chunk hash table (array)
 * @param chunk_hash_table: chunk hash table to be freed
 * @returns void
 **/
void free_chunk_hash_table(unsigned char ** chunk_hash_table)
{
	int i;
	int chunk_count = 0;
	
	for(i = 0;chunk_hash_table[i]!=NULL; i++)
	{
		chunk_count++;
	}
	
	for(i = 0;i < chunk_count;i++)
		free(chunk_hash_table[i]);
	
	free(chunk_hash_table);
	
	return;
}

/**
 * @brief Prints out the SHA256 hash of an entire file
 * @param hash : SHA256 file hash (unsigned character string)
 * @returns void
 **/
void printh(unsigned char * hash)
{
	if(hash == NULL) 
		die("File hash is null");
		
	int i;
	
	printf("FILE HASH : ");
	
	for(i = 0;i < SHA256_DIGEST_LENGTH; i++)
		printf("%02x", hash[i]);
	
	printf("\n");
	
	return;
}

/**
 * @brief Prints out the SHA256 hashes of all chunks from a chunk hush table
 * @param chunk_hash_table : The chunk hush table the hashes will be printed from (array of unsigned charcter strings)
 * @returns void
 **/
void printc(unsigned char ** chunk_hash_table)
{
	if(chunk_hash_table == NULL)
		die("Chunk hash table is empty (or null)");
		
	int i, j;
	int chunk_count = 0;
	
	for(i = 0;chunk_hash_table[i]!=NULL; i++)
	{
		chunk_count++;
	}
	
	if(chunk_count == -1)
		die("Unable to obtain chunk count to print chunk hash table");
		
	if(chunk_count == 1)
	{
		printf("No chunks (file too small)\n");
		return;
	}
	
	for(i = 0;i < chunk_count; i++)
	{
		printf("CHUNK %d : ", i+1);
		for(j = 0;j < SHA256_DIGEST_LENGTH; j++)
			printf("%02x", chunk_hash_table[i][j]);
		printf("\n");
	}
	
	return;
}
