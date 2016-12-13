#ifndef __Q12_CLIENT_H
#define __Q12_CLIENT_H

//Fichier h du client 

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
#include <ifaddrs.h>
#include <netdb.h>

#define _GNU_SOURCE

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

#endif
