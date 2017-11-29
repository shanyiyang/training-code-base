/**************************************************************/
/* This is a simple server.                                   */
/* Use unix domain socket and socket datagram to read data.   */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define MAXBUFFERSIZE 1024
#define SERVERIPC "/tmp/ipc_s.sock"
#define CLIENTIPC "/tmp/ipc_c.sock"

struct pass_value
{
	int num;
	char msg[100];
};
typedef struct pass_value pass_v;

int main()
{
	struct sockaddr_un 	sock_addr;
	struct sockaddr_un	addr_src;
	socklen_t 			addr_size;
	int 				local_sock;
	int 				read_size = 0;
	int 				rand_num;
	int 				count = 0;
	char				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];
	pass_v				sendmsg;
	pass_v				receivemsg;
	
	// Initial random number.
	srand(time(NULL));
	
	// Socket address for binding
	memset(&sock_addr, 0, sizeof(struct sockaddr_un));
	sock_addr.sun_family = PF_UNIX;
	strcpy(sock_addr.sun_path, SERVERIPC);
	
	unlink(SERVERIPC);
	
	// socket()
	if ((local_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	if(bind(local_sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
	{
		perror("Bind");
		exit(1);
	}
	printf("Bind socket successful!\n");
	
	/* Procedure */
	while (count < 10)
	{
		// Read a data from socket
		read_size = recvfrom(local_sock, inbuf, MAXBUFFERSIZE, 0, 
				(struct sockaddr*) &addr_src, (socklen_t *) &addr_size);
		memcpy(&receivemsg, inbuf, read_size);
		printf("%s %d.\n", receivemsg.msg, receivemsg.num);

		// Write a data by socket
		rand_num = rand() % 100;
		printf("Send number: %d\n", rand_num);
		sendmsg.num = rand_num;
		memcpy(sendmsg.msg, "Server message is", sizeof("Server message is"));
		memcpy(outbuf, &sendmsg, sizeof(sendmsg));
		sendto(local_sock, outbuf, sizeof(sendmsg), 0, 
				(struct sockaddr*) &addr_src, sizeof(addr_src));
				
		count ++;
	}
	
	close(local_sock);
	unlink(SERVERIPC);
	unlink(CLIENTIPC);
	return 0;
}
