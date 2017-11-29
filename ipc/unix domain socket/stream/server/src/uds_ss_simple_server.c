/**************************************************************/
/* This is a simple server.                                   */
/* Use unix domain socket and socket stream to read data.   */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
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
	struct sockaddr_un 	sock_addr;
	struct sockaddr_un	addr_remote;
	socklen_t 			addr_size;
	int 				local_sock;
	int					remote_sock;
	int 				read_size = 0;
	int 				rand_num;
	int 				count = 0;
	char				outbuf[MAXBUFFERSIZE];
	char				inbuf[MAXBUFFERSIZE];
	pass_v				sendmsg;
	pass_v				receivemsg;
	
	// Initial random number.
	srand(time(NULL));
	
	memset(&sock_addr, 0, sizeof(struct sockaddr_un));
	sock_addr.sun_family = PF_UNIX;
	strcpy(sock_addr.sun_path, SERVERIPC);

	unlink(SERVERIPC);
	
	// socket()
	if ((local_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Create socket successful!\n");
	
	// bind()
	if (bind(local_sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
	{
		perror("Bind");
		exit(1);
	}
	printf("Bind socket successful!\n");
	
	// listen()
	if (listen(local_sock, 5) == -1) {
        perror("listen");
        exit(1);
    }
	printf("Listen socket successful!\n");
	
	// accept()
	if ((remote_sock = accept(local_sock, (struct sockaddr *)&addr_remote, &addr_size)) < 0)
	{
		perror("Accept");
        exit(1);
	}
	printf("Accept socket successful!\n");
	
	/* Procedure */
	while (count < 10)
	{
		// Read a data from socket
		read_size = recv(remote_sock, inbuf, MAXBUFFERSIZE, 0);
		memcpy(&receivemsg, inbuf, read_size);
		printf("%s %d.\n", receivemsg.msg, receivemsg.num);

		// Write a data by socket
		rand_num = rand() % 100;
		printf("Send number: %d\n", rand_num);
		sendmsg.num = rand_num;
		memcpy(sendmsg.msg, "Server message is", sizeof("Server message is"));
		memcpy(outbuf, &sendmsg, sizeof(sendmsg));
		send(remote_sock, outbuf, sizeof(sendmsg), 0);
				
		count ++;
	}
	
	close(remote_sock);
	close(local_sock);
	unlink(SERVERIPC);
	return 0;
}
