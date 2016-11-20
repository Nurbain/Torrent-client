#include "q12_tracker.h"

int get_ip_ver(char * address)
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

int main(int argc, char ** argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Argument(s) mismatch!\nUsage: %s <IPV6_ADDR> <PORT>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	int port = atoi(argv[2]);
	int addr_length;
	int domain = get_ip_ver(argv[i]);
	int bytes_received;
	int sockfd;
	
	switch(domain)
	{
		case 6:
			void * message = (void*) malloc(PUT110_IPV6_SIZE*sizeof(void));
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
				die("Unable to bind socket with address parameters");
			}
			
			printf("Listening on %s port %d\n", argv[1], port);

			do
			{
				bytes_received = recvfrom(sockfd, message, PUT110_IPV6_SIZE, 0, (struct sockaddr *) &remote, addr_length);
				
				if(bytes_received == -1)
					die("Receiving interrupted due to an error");
				
				
			}
			while(bytes_received > 0);
	}
	
	
	
		
	return 0;
}
