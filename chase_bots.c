#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


#include "chase.h"


int random_key(int i)
{
    srand(time(NULL) + i);
    int keys[4] = {KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_LEFT};

    return keys[random()%4];   

}


int main(int argc, char** argv){

    int n_bots;

    if(argc != 2)
    {
        printf("Number of bots not provided!\n");
        exit(EXIT_FAILURE);
    }
    n_bots = atoi(argv[1]);

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
	int err = bind(sock_fd, (struct sockaddr *)&local_client_addr,
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
    int nbytes;

    msg_send.type = Connect_bots;

    msg_send.n_bots = n_bots;

    nbytes = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));


    msg_send.type = Bot_movement;

    while(1)
    {
        sleep(3);
        for(int i = 0; i < n_bots; i++)
        {
            msg_send.key[i] = random_key(i);
            printf("%d\n",msg_send.key[i]);
        }

        nbytes = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
    }
}