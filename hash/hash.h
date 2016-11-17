/**
 * @file hash.h
 * @brief Hash functions header file
 * @author Marek Felsoci
 **/
 
#ifndef _HASH__H
#define _HASH__H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <errno.h>

#define CHUNK_SIZE_IN_BYTES 1048576 // 1 Megabyte = 1 048 576 bytes
#define BUFFER_SIZE 1024 // 1 Kilobyte = 1024 bytes

void die(char * error_message);
unsigned char * hash_entire_file(char * filename);
int get_chunk_count(char * filename);
unsigned char ** chunk_and_hash(char * filename);
void free_chunk_hash_table(unsigned char ** chunk_hash_table);
void printh(unsigned char * hash);
void printc(unsigned char ** chunk_hash_table);

#endif
