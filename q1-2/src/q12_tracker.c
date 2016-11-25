#include "../include/q12_tracker.h"
#include "../../hash/src/hash.c"

//Détection du type d'addresse IPV4/IPV6 (pas utilisée pour l'instant)
int get_ip_ver(const char * address)
{
	if(address == NULL)
		die("Address pointer is null");
	
	struct addrinfo hint, *res = NULL;

    memset(&hint, '\0', sizeof(hint));

    hint.ai_family = PF_UNSPEC;
    hint.ai_flags = AI_NUMERICHOST;
    
    if (getaddrinfo(address, NULL, &hint, &res))
        die("Unknown address type");

    if(res->ai_family == AF_INET) 
    {
		freeaddrinfo(res);
        return 4;
    } 
    else if (res->ai_family == AF_INET6) 
    {
		freeaddrinfo(res);
        return 6;
    }

	freeaddrinfo(res);
	return -1;
}

//Conversion du message reçu en format exploitable (cf. fichier en-tête)
PGMessage vtopgm(void * message)
{
	if(message == NULL)
		die("Message does not exist");
	
	PGMessage result;
	
	memcpy(&result.ms_type, message, MESSAGE_TYPE_LENGTH);
	memcpy(&result.ms_size, message + MESSAGE_TYPE_LENGTH, MESSAGE_SIZE_LENGTH);
	
	void * hash = message + 2*(MESSAGE_TYPE_LENGTH + MESSAGE_SIZE_LENGTH);
	
	memcpy(result.ms_hash, hash, HASH_SIZE);
	
	void * port = hash + HASH_SIZE + MESSAGE_TYPE_LENGTH + MESSAGE_SIZE_LENGTH;
	
	memcpy(&result.ms_client.cl_port, port, MESSAGE_PORT_LENGTH);
	
	void * ipa = port + MESSAGE_PORT_LENGTH;
	
	memcpy(&result.ms_client.cl_addr, ipa, MESSAGE_IPV6_ADDRESS_LENGTH);
	
	return result;	
}

//Ajout d'un couple hash+client dans la liste
FileList fadd(FileList fl, const PGMessage message)
{
	if(message.ms_type != 110 || message.ms_type != 112)
		die("Message type incorrect");
	
	FileList new = (FileList) malloc(sizeof(struct s_flist));
	
	new->fl_hash = message.ms_hash;
	new->fl_client = message.ms_client;
	
	new->next = fl;
	
	return new;
}

//Ajout d'un couple hash+client dans la liste (prototype différent)
FileList sfadd(FileList fl, FileList new)
{
	if(new == NULL)
		return fl;
	
	FileList copy = (FileList) malloc(sizeof(struct s_flist));
	
	copy->fl_hash = memcpy(copy->fl_hash, new->fl_hash, HASH_SIZE);
	copy->fl_client = new->fl_client;
		
	copy->next = fl;
	
	return copy;
}

//Cherche un client dans la liste des couples hash+client à partir d'un hash
FileList srcm(FileList fl, unsigned char * file_hash)
{
	if(fl == NULL)
		return NULL;
	
	FileList temp = fl;
	FileList results = NULL;
	
	while(temp != NULL)
	{
		if(memcmp(file_hash, fl->fl_hash, HASH_SIZE) == 0)
			results = sfadd(results, temp); 
		
		temp = temp->next;
	}
	
	return results;
}

//Recupère les données d'un message PUT reçu et génère en sortie un message PUT ACK
void * put(void * message, FileList list)
{
	if(message == NULL)
		die("Empty message");
	
	list = fadd(list, vtopgm(message));
	
	void * output = message;
	
	char type = 111;
	
	memcpy(output, &type, MESSAGE_TYPE_LENGTH);
	
	return output;
}

//Retourne le cardinal de la liste de couples hash+client
int get_fl_size(FileList fl)
{
	if(fl == NULL)
		return 0;
	
	int r = 0;
	
	FileList temp = fl;
	
	while(temp != NULL)
	{
		r++;
		temp = temp->next;
	}
	
	return r;
}

//Recupère les données d'un message GET reçu et génère en sortie un message GET ACK
void * get(void * message, FileList list)
{
	char type = 113;
	
	PGMessage mget = vtopgm(message);
	
	FileList dcl = srcm(list, mget.ms_hash);
	
	int cc = get_fl_size(dcl);
	
	short int size = HASH_SEGMENT_SIZE + cc * CLIENT_SEGMENT_SIZE + MESSAGE_TYPE_LENGTH + MESSAGE_SIZE_LENGTH;
	
	void * output = malloc((size_t) size);
	
	if(dcl == NULL)
	{
		size = HASH_SEGMENT_SIZE + MESSAGE_TYPE_LENGTH + MESSAGE_SIZE_LENGTH;
		memcpy(output, &type, MESSAGE_TYPE_LENGTH);
		memcpy(output+MESSAGE_TYPE_LENGTH, &size, MESSAGE_SIZE_LENGTH);
		memcpy(output+MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH, &mget.ms_hash, HASH_SIZE);
		
		fprintf(stderr, "No clients sharing this file found\n");
		
		return output;
	}
	
	FileList temp = dcl;
	
	char type_hash = 50;
	
	memcpy(output, &type, MESSAGE_TYPE_LENGTH);
	memcpy(output+MESSAGE_TYPE_LENGTH, &size, MESSAGE_SIZE_LENGTH);
	memcpy(output+MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH, &type_hash, MESSAGE_TYPE_LENGTH);
	memcpy(output+(2*MESSAGE_TYPE_LENGTH)+MESSAGE_SIZE_LENGTH, &mget.ms_hash, HASH_SIZE);
	
	void * cstart = output+(2*MESSAGE_TYPE_LENGTH)+MESSAGE_SIZE_LENGTH+HASH_SIZE;
	
	char type_client = 55;
	short int client_length = CLIENT_MESSAGE_LENGTH;
	
	while(temp != NULL)
	{
		void * cl = malloc(CLIENT_SEGMENT_SIZE);
		
		memcpy(cl, &type_client, MESSAGE_TYPE_LENGTH);
		memcpy(cl+MESSAGE_TYPE_LENGTH, &client_length, MESSAGE_SIZE_LENGTH);
		memcpy(cl+MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH, &temp->fl_client.cl_port, MESSAGE_PORT_LENGTH);
		memcpy(cl+MESSAGE_TYPE_LENGTH+MESSAGE_SIZE_LENGTH+MESSAGE_PORT_LENGTH, &temp->fl_client.cl_addr, MESSAGE_IPV6_ADDRESS_LENGTH);
		
		memcpy(cstart, cl, CLIENT_SEGMENT_SIZE);
		
		free(cl);
		
		cstart = cstart+CLIENT_SEGMENT_SIZE;
		
		temp = temp->next;
	}
	
	return output;	
}

//Main
int main(int argc, char ** argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "Argument(s) mismatch!\nUsage: %s <IPV6_ADDR> <PORT>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	int port = atoi(argv[2]);
	socklen_t addr_length;
	//int domain = get_ip_ver(argv[1]);
	int bytes_received;
	int sockfd, ssockfd;
	FileList list = NULL;
	
	void * message = (void*) malloc(PGT_IPV6_SIZE*sizeof(void));
	struct sockaddr_in6 local, remote;
	
	local.sin6_family = AF_INET6;
	local.sin6_port = htons(port);
	addr_length = sizeof(struct sockaddr_in6);
	
	if(inet_pton(AF_INET6, argv[1], &local.sin6_addr) == -1)
		die("Unable to parse IPv6 address");
	
	if((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("Unable to launch retrieving socket");
	
	if(bind(sockfd, (struct sockaddr *) &local, addr_length) == -1)
	{
		close(sockfd);
		die("Unable to bind retrieving socket with address parameters");
	}
	
	printf("Listening on %s port %d\n", argv[1], port);

	while(1)
	{
		bytes_received = recvfrom(sockfd, message, PGT_IPV6_SIZE, 0, (struct sockaddr *) &remote, &addr_length);
		
		if(bytes_received == -1)
			die("Receiving interrupted due to an error");
			
		PGMessage recm = vtopgm(message);
		
		switch(recm.ms_type)
		{
			case 110: //PUT
				remote.sin6_family = AF_INET6;
				remote.sin6_port = htons(recm.ms_client.cl_port);
				remote.sin6_addr = recm.ms_client.cl_addr;
				
				if((ssockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
					die("Unable to launch sending socket");
				
				if(bind(ssockfd, (struct sockaddr *) &remote, addr_length) == -1)
				{
					close(ssockfd);
					die("Unable to bind sending socket with address parameters");
				}
				
				if(sendto(ssockfd, put(message, list), PGT_IPV6_SIZE, 0, (struct sockaddr *) &remote, addr_length) == -1)
				{
					close(ssockfd);
					die("Unable to send ACK message");
				}
				
				close(ssockfd);
			case 112: //GET
				remote.sin6_family = AF_INET6;
				remote.sin6_port = htons(recm.ms_client.cl_port);
				remote.sin6_addr = recm.ms_client.cl_addr;
				
				if((ssockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
					die("Unable to launch sending socket");
				
				if(bind(ssockfd, (struct sockaddr *) &remote, addr_length) == -1)
				{
					close(ssockfd);
					die("Unable to bind sending socket with address parameters");
				}
				
				if(sendto(ssockfd, get(message, list), PGT_IPV6_SIZE, 0, (struct sockaddr *) &remote, addr_length) == -1)
				{
					close(ssockfd);
					die("Unable to send ACK message");
				}
				
				close(ssockfd);
			default:
				die("Message received is of an unsupported type");
				
		}		
	}
	
	close(sockfd);	
		
	return 0;
}
