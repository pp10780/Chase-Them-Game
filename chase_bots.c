#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>



#include "chase.h"


int random_key(int i)
{
    srand(time(NULL) + i);
    int keys[4] = {KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_LEFT};

    return keys[random()%4];   

}


int main(int argc, char** argv){

    int n_bots;
    int err;
    time_t start;

    if(argc != 3)
    {
        printf("Invalid input arguments\nFORMAT: ./{EXECUTABLE} {N_BOTS} /tmp/sock_16\n");
        exit(EXIT_FAILURE);
    }
    n_bots = atoi(argv[1]);

    if(n_bots > 10 || n_bots < 1)
    {
        printf("Invalid number of bots!\n");
        exit(EXIT_FAILURE);
    }

    printf("My pid is %d (no other proces has the same pid :)\n", getpid());
	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_client_addr;
	local_client_addr.sun_family = AF_UNIX;
	sprintf(local_client_addr.sun_path, "%s_%d", argv[2], getpid());

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
	strcpy(server_addr.sun_path, argv[2]);

    message_c2s msg_send;


    msg_send.type = Connect_bots;

    msg_send.n_bots = n_bots;

    err = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
    if(err == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));            
        exit(0);
    }


    msg_send.type = Bot_movement;

    
    for(int i = 0; i < n_bots; i++)
    {
        msg_send.key[i] = random_key(i);
        printf("%d\n",msg_send.key[i]);
    }
    start = time(NULL);
    while(1)
    {   
        if(time(NULL) - start >= 3)
        {
            
            err = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
            if(err == -1)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));            
                exit(0);
            }
            start = time(NULL);
            for(int i = 0; i < n_bots; i++)
            {
                msg_send.key[i] = random_key(i);
                printf("%d\n",msg_send.key[i]);
            }
        }
        
    }
}