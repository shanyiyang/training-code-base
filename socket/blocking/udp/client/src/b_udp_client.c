/*********************************************/
/* This is a simple UDP client.              */
/* Use blocking function to read data.       */
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

#define MAXBUFFERSIZE 1024

int main()
{
	/* Define variable */
	struct sockaddr_in 	sock_addr;
	struct sockaddr_in	addr_src;
	struct sockaddr_in	addr_dst;
	int 				local_sock;
	socklen_t 			addr_c_size;
	int					read_size;
	int 				rand_num;
	int 				count = 0;
	int 				read_num;
	char				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];

	// Initial random number.
	srand(time(NULL));
	
	/* Initial variable */
	// Initial socket variable
	memset(&sock_addr, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	sock_addr.sin_family = AF_INET;							// AF_INET: Address Family
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");		// IP address
	sock_addr.sin_port = htons(5678);							// Port number

	// Create socket
	if ((local_sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");

	/* Procedure */
	
	while(count < 10)
	{
		rand_num = rand() % 100;
		printf("Send number: %d\n", rand_num);
		memcpy(outbuf, &rand_num, sizeof(int));
		sendto(local_sock, outbuf, sizeof(int), 0, 
				(struct sockaddr*) &sock_addr, sizeof(addr_dst));

		read_size = recvfrom(local_sock, inbuf, sizeof(int), 0, 
				(struct sockaddr*) &addr_src, &addr_c_size);
		memcpy(&read_num, inbuf, read_size);
		printf("Read number: %d\n", read_num);
		count++;
	}
	
	close(local_sock);
	
	return 0;
}
