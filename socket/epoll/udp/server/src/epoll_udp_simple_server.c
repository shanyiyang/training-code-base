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
#include <sys/epoll.h>

#define MAXBUFFERSIZE 1024
#define MAXEVENT 10

int main()
{
	/* Define variable */
	struct sockaddr_in	sock_addr;
	struct sockaddr_in 	addr_src;
	int 				local_sock;							// This socket is used for client to connect.
	socklen_t 			addr_size;
	int 				read_size = 0;
	int 				rand_num;
	int 				count = 0;
	int 				read_num;
	int 				i;
	int 				epollfds;
	int 				ret_num;
	char 				outbuf[MAXBUFFERSIZE];
	char 				inbuf[MAXBUFFERSIZE];
	struct epoll_event 	event;
	struct epoll_event	ev_list[MAXEVENT];
	
	/* Initial variable */
	// Initial epll variables.
	if ((read_num = epollfds = epoll_create(MAXEVENT)) < 0)
	{
		perror("Epoll create");
		exit(1);
	}
	
	// Initial random number.
	srand(time(NULL));
	
	// Initial variables.
	memset(&sock_addr, 0, sizeof(struct sockaddr_in));		// Clear struct sockaddr_in variable
	sock_addr.sin_family = AF_INET;						// AF_INET: Address Family
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	sock_addr.sin_port = htons(5678);						// Port number
	
	// socket()
	if ((local_sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// Bind()
	if (bind(local_sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	printf("Bind socket successful!\n");
	
	// monitor socket
	event.data.fd = local_sock;
	event.events = EPOLLIN | EPOLLET;
	if ((ret_num = epoll_ctl(epollfds, EPOLL_CTL_ADD, local_sock, &event)) < 0)
	{
		perror("Epoll ctl add server socket\n");
		exit(1);
	}
	
	/* Procedure */
	while (count < 10)
	{
		if ((ret_num = epoll_wait(epollfds, ev_list, MAXEVENT,-1)) < 0)
		{
			if (errno == EINTR)
				continue;
			perror("Epoll wait");
		}
		else
		{
			for (i = 0; i < ret_num; i ++)
			{
				if (ev_list[i].events == EPOLLERR  || ev_list[i].events == EPOLLHUP )
				{
					perror("Epoll wait");
					exit(1);
				}
				else
				{
					// Read a data from socket.
					read_size = recvfrom(ev_list[i].data.fd, inbuf, sizeof(int), 0, 
							(struct sockaddr*) &addr_src, &addr_size);
					memcpy(&read_num, inbuf, read_size);
					printf("Read number: %d\n", read_num);
							
					// Write a data by socket
					rand_num = rand() % 100;
					printf("Send number: %d\n", rand_num);
					memcpy(outbuf, &rand_num, sizeof(int));
					sendto(ev_list[i].data.fd, outbuf, sizeof(int), 0, 
							(struct sockaddr*) &addr_src, sizeof(addr_src));
							
					count ++;
				}
			}
		}
	}
	
	close(local_sock);
	
	return 0;
}
