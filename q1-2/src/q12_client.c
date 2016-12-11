#include "../include/q12_client.h"

int main(int argc , char **argv)
{
	int sockfd;
	socklen_t addrlgn;
	struct sockaddr_in6 dest;


	//Création du socket 
	if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
	{
		perror("socket fail \n");
		exit(EXIT_FAILURE);
	}


	//creation adresse destination
	dest.sin6_family = AF_INET6;
    	dest.sin6_port   = htons(atoi(argv[2]));
    	addrlgn         = sizeof(struct sockaddr_in6);

	if(inet_pton(AF_INET6, argv[1], &dest.sin6_addr) != 1)
    {
        perror("inet fail \n");
		close(sockfd);
		exit(EXIT_FAILURE);
    }

	//fichier
	unsigned char hash_type = 50;
	short int hash_lgn = strlen(argv[5])+1;
	char hash[strlen(argv[5])+1];
	strcpy(hash , argv[5]);

	//client
	unsigned char client_type=55;
	short int client_lgn = sizeof(dest.sin6_addr)+2;
	short int client_port =atoi(argv[2]);
	struct in6_addr client_addr = dest.sin6_addr;
	

	//message
	unsigned char msg_type;
	if(strcmp(argv[4], "put" ) ==0)
	{
		msg_type = 110;
	}

	if (strcmp(argv[4] , "get") == 0)
	{
		msg_type = 112;
	}

	short int msg_lgn = 1+2+hash_lgn+1+2+client_lgn;
	int total_lgn = 1+2+msg_lgn;
	void* buf = malloc(total_lgn);


	//placement dans buffeur


	memcpy(buf, &msg_type, sizeof(msg_type)); //type message
	memcpy(buf+sizeof(msg_type), &msg_lgn , sizeof(msg_lgn)); // taille message
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn), &hash_type , sizeof(hash_type)); //type hash
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type) , &hash_lgn , sizeof(hash_lgn)); // taille hash
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn), hash , sizeof(hash)); //hash
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash), &client_type , sizeof(client_type)); // type client
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type), &client_lgn , sizeof(client_lgn)); // taille client
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn), &client_port , sizeof(client_port)); //port client
	memcpy(buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn)+sizeof(client_port), & client_addr , sizeof(client_addr)); //adresse client


	char pmsg_type;
	short int pmsg_lgn;
	char phash_type;
	short int phash_lgn;
	char phash[65];
	char pclient_type;
	short int pclient_lgn;
	short int pclient_port;
	char * client = (char*)malloc(50);
	struct in6_addr pclient_addr;

	memcpy(&pmsg_type , buf , sizeof(pmsg_type)); //type message
	memcpy(&pmsg_lgn , buf+sizeof(msg_type),sizeof(pmsg_lgn)); //taille message
	memcpy(&phash_type , buf+sizeof(msg_type)+sizeof(msg_lgn), sizeof(phash_type));  //type hash
	memcpy(&phash_lgn , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type), sizeof(phash_lgn)); //taille hash
	memcpy(phash , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn), sizeof(phash)); // hash
	memcpy(&pclient_type , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash) , sizeof(pclient_type)); // type client
	memcpy(&pclient_lgn , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type),sizeof(pclient_lgn)); // taille client
	memcpy(&pclient_port , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn), sizeof(pclient_port));
	memcpy(&pclient_addr , buf+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn)+sizeof(pclient_port), sizeof(pclient_addr));


	//Print affichage donner 

	printf("\nMessage à envoyer : \n");
	printf("type msg : %u \n",pmsg_type);
	printf("longeur msg : %d \n",pmsg_lgn);
	printf("type hash : %u \n ", phash_type);
	printf("longeur hash : %d \n", phash_lgn);
	printf("hash : %s \n", phash);
	printf("type client : %u \n",pclient_type);
	printf("longuer client : %d \n",pclient_lgn);
	printf("port client : %d \n", pclient_port);
	printf("client : %s \n" , inet_ntop(AF_INET6 , &pclient_addr,client,sizeof(pclient_addr)));


	//envoi du message 

	if (sendto(sockfd,buf, total_lgn, 0 , (struct sockaddr * ) &dest , addrlgn ) == -1 )
	{
		perror("socket fail \n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}


	int sockfd2; //descripteur
	socklen_t addrlgn2; //socket

	struct sockaddr_in6 my_addr; // adresse ipv4

	//socket a creer
	if((sockfd2 = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
	{
		perror("socket fail \n");
		exit(EXIT_FAILURE);
	}

	//initialisation de l'adresse local
	my_addr.sin6_family = AF_INET6; // domaine de l'adresse 
	my_addr.sin6_port = htons(atoi(argv[3]));	//le port
	my_addr.sin6_addr = in6addr_any; //on ecoute sur une adresse lambda
	addrlgn2 = sizeof(struct sockaddr_in6);

	//association de a socket avec 'adresse
	if (bind(sockfd2, (struct sockaddr *) &my_addr, addrlgn2)== -1) // adress
	{
		perror("bind fail \n");
		close(sockfd2);
		exit(EXIT_FAILURE);
	}

	if (!strcmp(argv[4],"put") || !strcmp(argv[4],"get"))
	{
		void* ack = malloc(total_lgn+1000);
		
	//reception de chaine de caractère 

		if(recvfrom(sockfd2,ack,total_lgn+1000 ,0 , (struct sockaddr *) &my_addr , &addrlgn2 ) == -1)
		{
			perror("recois fail \n");
			close(sockfd2);
			exit(EXIT_FAILURE);
		}
	

		char rmsg_type;
		short int rmsg_lgn;
		char rhash_type ;
		short int rhash_lgn;
		char rhash[65];
		char rclient_type;
		short int rclient_lgn;
		short int rclient_port ;
		struct in6_addr rclient_addr;

		memcpy(&rmsg_type, ack , sizeof(pmsg_type)); //type message
		memcpy(&rmsg_lgn , ack+sizeof(msg_type), sizeof(pmsg_lgn)); //taille message
		memcpy(&rhash_type, ack+sizeof(msg_type)+sizeof(msg_lgn), sizeof(phash_type));  //type hash
		memcpy(&rhash_lgn, ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type),sizeof(phash_lgn)); //taille hash
		memcpy(rhash, ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn), sizeof(phash)); //hash
		memcpy(&rclient_type, ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+ sizeof(hash), sizeof(pclient_type)); //type client
		memcpy(&rclient_lgn,ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+ sizeof(hash) + sizeof(client_type), sizeof(pclient_lgn)); //taille client
		memcpy(&rclient_port,ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+ sizeof(hash) + sizeof(client_type)+sizeof(client_lgn) ,sizeof(pclient_port)); 
		memcpy(&rclient_addr,ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+ sizeof(hash) + sizeof(client_type)+sizeof(client_lgn)+sizeof(client_port), sizeof(pclient_addr));


		printf("\nReponse : \n");
		printf("type msg : %u \n",rmsg_type);
		printf("longeur msg : %d \n",rmsg_lgn);
		printf("type hash : %u \n ", rhash_type);
		printf("longeur hash : %d \n", rhash_lgn);
		printf("hash : %s \n", rhash);
		printf("type client : %u \n",rclient_type);
		printf("longuer client : %d \n",rclient_lgn);
		printf("port client : %d \n", rclient_port);
		printf("client : %s \n" , inet_ntop(AF_INET6 , &rclient_addr,client,sizeof(rclient_addr)));


		int s = 21;
		while (rmsg_lgn > 89)
		{
			memcpy(&rclient_type , s+ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash) , sizeof(pclient_type)); //type client
			memcpy(&rclient_lgn , s+ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type), sizeof(pclient_lgn)); //taille client
			memcpy(&rclient_port, s+ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn), sizeof(pclient_port));
			memcpy(&rclient_addr,s+ack+sizeof(msg_type)+sizeof(msg_lgn)+sizeof(hash_type)+sizeof(hash_lgn)+sizeof(hash)+sizeof(client_type)+sizeof(client_lgn)+sizeof(client_port),sizeof(pclient_addr));
			s+=21;
			rmsg_lgn-=21;

			printf("\n type client : %u \n" , rclient_type);
			printf("longeur client : %d \n" , rclient_lgn);
			printf("port client : %d \n " , rclient_port);
			printf("client : %s \n" , inet_ntop(AF_INET6 , &rclient_addr,client,sizeof(rclient_addr)));
		}
	}
	
	return 0;
}
























