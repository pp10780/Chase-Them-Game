#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"
#include "structs.h"


int main()
{

	int err;
    printf("My pid is %d (no other proces has the same pid :)\n", getpid());
	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_client_addr;
	local_client_addr.sun_family = AF_UNIX;
	sprintf(local_client_addr.sun_path, "%s_%d", SOCK_ADDRESS, getpid());

	printf("this process address is %s\n", local_client_addr.sun_path);
	unlink(local_client_addr.sun_path);
	err = bind(sock_fd, (struct sockaddr *)&local_client_addr,
							sizeof(local_client_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    printf("Socket created \nReady to send\nReady to recieve\n");

    struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

    message_c2s msg_send;

    msg_send.type = New_Prize;


    while(1)
    {
        sleep(5);
        err = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
		if(err == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));            
            exit(0);
        }
           
    }
}