/****************************************************/
/* This is a simple TCP server.                     */
/* Use select and blocking function to read data.   */
/****************************************************/

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
	struct sockaddr_in 	addr_local;
	struct sockaddr_in	addr_remote;
	int 				local_sock;							// This socket is used for client to connect.
	int					remote_sock;						// If a new client is accepted by this server, 
															// this socket is used for server to communicate with the new client.
	int 				addr_size = sizeof(addr_remote);
	int 				read_size = 0;
	int 				rand_num;
	int 				count = 0;
	int 				read_num;
	int 				maxfds;
	int 				i;
	char 				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];
	fd_set 				active_fds;
	fd_set				read_fds;
	
	// Initial seed.
	srand(time(NULL));
	
	/* Initial variable */
	// Initial variable.
	memset(&addr_local, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	addr_local.sin_family = AF_INET;						// AF_INET: Address Family
	addr_local.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	addr_local.sin_port = htons(5678);						// Port number
	
	// scoket().
	if ((local_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// Bind()
	if (bind(local_sock, (struct sockaddr *) &addr_local, sizeof(addr_local)) == -1)
	{
		perror("bind");
		exit(1);
	}
	printf("Bind socket successful!\n");

	// Listen()	
	if (listen(local_sock, 5) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("Listen socket successful!\n");
	
	// Monitor fds.
	FD_ZERO(&active_fds);
	FD_SET(local_sock, &active_fds);
	maxfds = local_sock;
	
	while (count < 10)
	{
		read_fds = active_fds;
		if (select(maxfds + 1, &read_fds, NULL, NULL, NULL) < 0)
		{
			perror("Select");
		}
		else
		{
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &read_fds))
				{
					if (i == local_sock)
					{
						// Accept()
						if ((remote_sock = accept(local_sock, (struct sockaddr *) &addr_remote, (socklen_t *) &addr_size)) == -1)
						{
							perror("Accept");
							exit(1);
						}
						printf("Accept socket successful!\n");
						FD_SET(remote_sock, &active_fds);
						maxfds++;
						read_fds = active_fds;
					}
					else
					{
						// Read a data from socket.
						read_size = recv(i, inbuf, sizeof(int), 0);
						memcpy(&read_num, inbuf, read_size);
						printf("Read number: %d\n", read_num);
						
						// Write a data by socket
						rand_num = rand() % 100;
						printf("Send number: %d\n", rand_num);
						memcpy(outbuf, &rand_num, sizeof(int));
						send(remote_sock, outbuf, sizeof(int), 0);
						
						count ++;
					}
				}
			}
		}
	}
	
	close(remote_sock);
	close(local_sock);
	
	return 0;
}
