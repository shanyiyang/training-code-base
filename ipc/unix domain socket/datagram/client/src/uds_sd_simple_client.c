/**************************************************************/
/* This is a simple client.                                   */
/* Use unix domain socket and socket datagram to read data.   */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAXBUFFERSIZE 1024
#define SERVERIPC "/tmp/ipc_s.sock"

struct pass_value
{
	int num;
	char msg[100];
};
typedef struct pass_value pass_v;

int main()
{
	/* Define variable */
	struct sockaddr_un 	sock_addr;
	struct sockaddr_un 	addr_src;
	struct sockaddr_un 	addr_dst;
	int 				local_sock;
	socklen_t 			addr_size;
	int					read_size;
	int 				rand_num;
	int 				count = 0;
	char				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];
	pass_v				sendmsg;
	pass_v				receivemsg;
	
	// Initial random number.
	srand(time(NULL));
	
	/* Initial variable */
	
	// Socket address for binding
	memset(&sock_addr, 0, sizeof(struct sockaddr_un));
	sock_addr.sun_family = PF_UNIX;
	strcpy(sock_addr.sun_path, CLIENTIPC);
	
	unlink(CLIENTIPC);
	
	// socket()
	if ((local_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");

	// Destination address
	memset(&addr_dst, 0, sizeof(struct sockaddr_un));
	addr_dst.sun_family = PF_UNIX;
	strcpy(addr_dst.sun_path, SERVERIPC);
	
	/* Procedure */
	while(count < 10)
	{
		// Write a data by socket
		rand_num = rand() % 100;
		printf("Send number: %d\n", rand_num);
		sendmsg.num = rand_num;
		memcpy(sendmsg.msg, "Client message is", sizeof("Server message is"));
		memcpy(outbuf, &sendmsg, sizeof(sendmsg));
		sendto(local_sock, outbuf, sizeof(sendmsg), 0, 
				(struct sockaddr*) &addr_dst, sizeof(addr_dst));
				
		// Read a data from socket
		read_size = recvfrom(local_sock, inbuf, MAXBUFFERSIZE, 0, 
				(struct sockaddr*) &addr_src, (socklen_t *) &addr_size);
		memcpy(&receivemsg, inbuf, read_size);
		printf("%s %d.\n", receivemsg.msg, receivemsg.num);
			
		count++;
	}
	
	close(local_sock);
	unlink(SERVERIPC);
	unlink(CLIENTIPC);

	return 0;
}
