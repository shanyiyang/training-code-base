/*********************************************/
/* This is a simple UDP client.              */
/* Use non-blocking function to read data.   */
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
	/* Define variable */
	struct sockaddr_in	sock_addr;
	struct sockaddr_in	addr_src;
	struct sockaddr_in 	addr_dst;
	int 				local_sock;							// This socket is used for client to communicate with server.
	int 				read_size;
	int 				rand_num;
	int 				count = 0;
	int 				read_num;
	socklen_t 			addr_size;
	int 				rwflag = 1;
	char				outbuf[MAXBUFFERSIZE];
	char 				inbuf[MAXBUFFERSIZE];

	// Initial random number.
	srand(time(NULL));
	
	/* Initial variable */
	// Server address struct
	memset(&sock_addr, 0, sizeof(struct sockaddr_in));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sock_addr.sin_port = htons(5678);

	// socket()
	if ((local_sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// set socket to non-blocking socket
	fcntl(local_sock, F_SETFL, O_NONBLOCK);

	/* Procedure */
	while(count < 10)
	{
		// Write a data by socket
		if (rwflag == 1)
		{
			// Write a data by socket
			rand_num = rand() % 100;
			printf("Send number: %d\n", rand_num);
			memcpy(outbuf, &rand_num, sizeof(int));
			sendto(local_sock, outbuf, sizeof(int), 0, 
				(struct sockaddr*) &sock_addr, sizeof(addr_dst));
			rwflag = 0;
		}
		else
		{
			// Read a data from socket
			read_size = recvfrom(local_sock, inbuf, sizeof(int), 0, 
				(struct sockaddr*) &addr_src, &addr_size);
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
			
			rwflag = 1;
			count++;
		}
	}
	
	close(local_sock);
	
	return 0;
}
