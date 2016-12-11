#ifndef __Q12_TRACKER_H
#define __Q12_TRACKER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../hash/hash.h"

//Définition de différentes tailles et longueurs (en octets-Bytes)
#define PGT_IPV6_SIZE 92 //Bytes
#define MESSAGE_TYPE_LENGTH 1
#define MESSAGE_SIZE_LENGTH 2
#define HASH_SIZE 65
#define MESSAGE_PORT_LENGTH sizeof(short int)
#define MESSAGE_IPV6_ADDRESS_LENGTH sizeof(struct in6_addr)
#define TRACKER_PUT_MESSAGE_SIZE 3*(MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH)+HASH_SIZE+MESSAGE_PORT_LENGTH+MESSAGE_IPV6_ADDRESS_LENGTH;
#define TRACKER_GET_MESSAGE_SIZE TRACKER_PUT_MESSAGE_SIZE
#define TRACKER_PUT_ACK_MESSAGE_SIZE TRACKER_PUT_MESSAGE_SIZE
#define HASH_SEGMENT_SIZE MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH+HASH_SIZE
#define CLIENT_SEGMENT_SIZE MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH+MESSAGE_PORT_LENGTH+MESSAGE_IPV6_ADDRESS_LENGTH
#define CLIENT_MESSAGE_LENGTH 18

//Définition du type client
typedef struct s_client
{
	short int cl_port;
	struct in6_addr cl_addr;
} client;

//Définition du type FileList (liste chaînée de couples hash+client)
typedef struct s_flist 
{
	unsigned char * fl_hash;
	client fl_client;
	struct s_flist * next;
} *FileList, filelist;

//Définition du type PGMessage (stockage de données en format exploitable des messages PUT/GET reçus)
typedef struct s_pgmess
{
	char ms_type;
	short int ms_size;
	unsigned char * ms_hash;
	client ms_client;
} PGMessage;

PGMessage vtopgm(void * message);
FileList fadd(FileList fl, const PGMessage message);
FileList sfadd(FileList fl, FileList new);
FileList srcm(FileList fl, unsigned char * file_hash);
int get_fl_size(FileList fl);
void * put(void * message, FileList list);
void * get(void * message, FileList list);
int get_ip_ver(const char * address);


#endif
