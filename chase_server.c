#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "chase.h"
  

// Users - Letter
// Bots - *
// Prizes - 1-5

// MAX HEALTH: 10 HP
// MAX PLAYERS: 10

// Crash (Keep same position) between:
// USERS: +1 HP (HIT), -1 HP (GOT HIT)
// USER AND BOT: -1 HP (BOT hits USER), Nothing (USER hits BOT)
// USER AND WALL: NOTHING
// USER AND PRIZE: User takes prize place and adds the prize number to its HP (MAX HP = 10)

// NEW PRIZE EVERY 5 SECONDS


// MESSAGES

// Connect
// Ball_Information
// Ball_Movement
// Field_Status
// Health_0
// Disconnect

void init_user(client_t* user){

	for (int i = 0; i < 10; i++){
		user[i].id = '-';
		user[i].pos[0] = -1;
		user[i].pos[1] = -1;
		user[i].hp = -1;
	}
}

void generate_valid_pos(field_t* field, int* pos){
	int val_pos = 0;
	srand(time(0));

	do{
		pos[0] = 1 + rand() % 19;
		pos[1] = 1 + rand() % 19;

		if (field[pos[0]*WINDOW_SIZE + pos[1]].status == -1) val_pos = 1;
	} while(!val_pos);

}

void init_prize(prize_t* prize)
{
	for(int i = 0; i < 10; i++)
	{
		prize[i].value = -1;
	}
}

void create_prize(field_t* field, prize_t* prize)
{
	for(int i = 0; i < 10; i++)
	{
		if(prize[i].value == -1)
		{
			generate_valid_pos(field, prize[i].pos);
			field[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].status = 2;
			field[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].idx = i;
			break;
		}
	}
}

void init_field(field_t* field){
	for (int i = 0; i < WINDOW_SIZE*WINDOW_SIZE; i++){
		field[i].status = -1;
	}
}

int create_user(client_t* user, field_t* field, message msg){
	int i;
	for(i = 0; i < 10; i++){
		if (user[i].id == '-'){
			user[i].id = msg.id;
			user[i].idx = i;
			generate_valid_pos(field, user[i].pos);
			user[i].hp = 10;
			field[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].status = 0;
			field[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].idx = i;
			break;
		}
	}
	return i;

}



int main(){

	field_status_t field_status;
	field_t field[WINDOW_SIZE*WINDOW_SIZE];
	int sock_fd;
	message msg;
	int nbytes;
	int idx;
	
	init_user(field_status.user);
	init_field(field);
	init_prize(field_status.prize);

	//for (int i = 0; i < 5; i++){
	//	create_prize(field, field_status.prize);
	//}
	

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_addr;
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	unlink(SOCK_ADDRESS);
	int err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

	printf(" socket created and binded \n ");
	printf("Ready to receive messages\n");
	
	struct sockaddr_un client_addr;
	socklen_t client_addr_size = sizeof(struct sockaddr_un);
	while(1){
		printf("Waiting...\n");
		nbytes = recvfrom(sock_fd, &msg, sizeof(message), 0,
		                  ( struct sockaddr *)&client_addr, &client_addr_size);

		if(msg.type == Connect){
			printf("OOGA BOOGA IN DA MOOGA");
			idx = create_user(field_status.user, field, msg);

			nbytes = sendto(sock_fd,
						&field_status.user[idx], sizeof(field_status.user), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
		}
		else if(msg.type == Ball_movement)
		{

		}
		else if(msg.type == Disconnect)
		{

		}
		else
		{
			printf("TRASH MESSAGE\n");
		}
	}

	close(sock_fd);
	//unlink(SOCK_ADDRESS);
	exit(0);
}