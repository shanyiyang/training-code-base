/*********************************************/
/* This is a simple TCP server.              */
/* Use no -blocking function to read data.   */
/*********************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAXBUFFERSIZE 1024

int main()
{
	/*
	 * Define variable
	 */
	struct sockaddr_in 	addr_local;
	struct sockaddr_in 	addr_remote;
	int 				local_sock;							// This socket is used for client to connect.
	int					remote_sock;						// If a new client is accepted by this server, 
															// this socket is used for server to communicate with the new client.
	int 				addr_size = sizeof(addr_remote);
	int 				read_size = 0;
	int 				rand_num;
	int					count = 0;
	int					read_num;
	int 				outbuf[MAXBUFFERSIZE];
	int 				inbuf[MAXBUFFERSIZE];
	
	// Initial random number.
	srand(time(NULL));
	
	/* Initial variable */
	// Initial socket variables.
	memset(&addr_local, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	addr_local.sin_family = AF_INET;						// AF_INET: Address Family
	addr_local.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	addr_local.sin_port = htons(5678);						// Port number
	
	// socket()
	if ((local_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// bind()
	if (bind(local_sock, (struct sockaddr *) &addr_local, sizeof(addr_local)) < 0)
	{
		perror("bind");
		exit(1);
	}
	printf("Bind socket successful!\n");
	
	// set non-blocking socket
	fcntl(local_sock, F_SETFL, O_NONBLOCK);

	// listen()	
	if (listen(local_sock, 5) < 0)
	{
		perror("listen");
		exit(1);
	}
	printf("Listen socket successful!\n");
	
	// accept()
	while ((remote_sock = accept(local_sock, (struct sockaddr *) &addr_remote, (socklen_t *) &addr_size)) < 0)
	{
		if (errno == EAGAIN)
			continue;
		perror("Accept");
		exit(1);
	}
	printf("Accept socket successful!\n");
	
	/* Procedure */
	while (count < 10)
	{
		// Read a data from socket
		read_size = recv(remote_sock, inbuf, sizeof(int), 0);
		if (read_size < 0)
		{
			if (errno == EAGAIN)
				continue;
			else
			{
				perror("Read server");
				exit(1);
			}
		}
		else
		{
			// Get the data from the buffer.
			memcpy(&read_num, inbuf, read_size);
			printf("Read number: %d\n", read_num);
				
			// Write a data by socket.
			rand_num = rand() % 100;
			printf("Send number: %d\n", rand_num);
			memcpy(outbuf, &rand_num, sizeof(int));
			send(remote_sock, outbuf, sizeof(int), 0);
				
			count++;
		}	
	}
	
	close(local_sock);
	close(remote_sock);
	
	return 0;
}
