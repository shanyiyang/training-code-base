/*********************************************/
/* This is a simple TCP client.              */
/* Use blocking function to read data.       */
/*********************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAXBUFFERSIZE 1024

int main()
{
	/* Define variable */
	struct sockaddr_in 	addr_local;
	int 				local_sock;							// This socket is used for client to communicate with server.
	int					read_size;
	int 				rand_num;
	int 				count = 0;
	int 				read_num;
	char				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];

	// Initial random seed.
	srand(time(NULL));
	
	/* Initial variable */
	// Initial socket variable
	memset(&addr_local, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	addr_local.sin_family = AF_INET;						// AF_INET: Address Family
	addr_local.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	addr_local.sin_port = htons(5678);						// Port number

	// socket()
	if ((local_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");

	// connect()
	if ((connect(local_sock, (struct sockaddr *)&addr_local, (socklen_t) sizeof(addr_local))) == -1)
	{
		perror("connect");
		exit(1);
	}
	printf("Connect server socket successful!\n");

	/* Procedure */
	while(count < 10)
	{
		// Write a data by socket.
		rand_num = rand() % 100;
		printf("Send number: %d\n", rand_num);
		memcpy(outbuf, &rand_num, sizeof(int));
		send(local_sock, outbuf, sizeof(int), 0);
		
		// Read a data from socket.
		read_size = recv(local_sock, inbuf, sizeof(int), 0);
		memcpy(&read_num, inbuf, read_size);
		printf("Read number: %d\n", read_num);
		
		count ++;
	}
	
	close(local_sock);
	
	return 0;
}
