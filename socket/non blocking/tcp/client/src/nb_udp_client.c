/*********************************************/
/* This is a simple TCP server.              */
/* Use no-blocking function to read data.    */
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
#include <fcntl.h>

#define MAXBUFFERSIZE 1024

int main()
{
	/* Define variable */
	struct sockaddr_in 	addr_remote;
	int 				local_sock;							// This socket is used for client to communicate with server.
	int					read_size;
	int					rand_num;
	int 				count = 0;
	int 				read_num;
	int 				rwflag = 1;
	int 				outbuf[MAXBUFFERSIZE];
	int 				inbuf[MAXBUFFERSIZE];

	/* 
	 *Initial variable 
	 */
	
	// Initial random number.
	srand(time(NULL));
	
	// Initial socket variables.
	memset(&addr_remote, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	addr_remote.sin_family = AF_INET;						// AF_INET: Address Family
	addr_remote.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	addr_remote.sin_port = htons(5678);						// Port number

	// socket()
	if ((local_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// connect
	if ((connect(local_sock, (struct sockaddr *)&addr_remote, (socklen_t) sizeof(addr_remote))) == -1)
	{
		perror("connect");
		exit(1);
	}
	printf("Connect server socket successful!\n");

	// set socket to non-blocking socket
	fcntl(local_sock, F_SETFL, O_NONBLOCK);

	/* Procedure */
	while(count < 10)
	{
		// Write a data by socket
		if (rwflag == 1)
		{
			rand_num = rand() % 100;
			printf("Send number: %d\n", rand_num);
			memcpy(outbuf, &rand_num, sizeof(int));
			send(local_sock, outbuf, sizeof(int), 0);
			
			// Set flag read and next round will read data.
			rwflag = 0;
		}
		else
		{
			read_size = recv(local_sock, inbuf, sizeof(int), 0);
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
			// Get the data from the buffer.
			memcpy(&read_num, inbuf, read_size);
			printf("Read number: %d\n", read_num);
			
			// Set flag write and next round will write data.
			rwflag = 1;
			count++;
		}
	}
	
	close(local_sock);
	
	return 0;
}
