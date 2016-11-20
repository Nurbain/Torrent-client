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
#include "../hash/hash.h"

#define PUT110_IPV4_SIZE 77 //Bytes
#define PUT110_IPV6_SIZE 89 //Bytes

int get_ip_ver(char * address);

#endif
