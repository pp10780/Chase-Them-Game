#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>

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


/******************************************************************************
 * init_client()
 *
 * Arguments: client - struct with all the client's info
 * Returns:
 *
 * Description: Initializes the client
 *****************************************************************************/
void init_client(client_t* client){

	for (int i = 0; i < 10; i++){
		client[i].id = '-';
		client[i].pos[0] = -1;
		client[i].pos[1] = -1;
		client[i].hp = -1;
		
	}
}

/******************************************************************************
 * generate_valid_pos()
 *
 * Arguments: map - vector with the info of all map position
			  pos - vector that has the x and y coordinates of a position
 * Returns:
 *
 * Description: Indicates an open position in the map
 *****************************************************************************/
void generate_valid_pos(position_t* map, int* pos){
	int val_pos = 0;
	srand(time(0));

	do{
		pos[0] = 1 + rand() % (WINDOW_SIZE - 2);
		pos[1] = 1 + rand() % (WINDOW_SIZE - 2);

		if (map[pos[0]*WINDOW_SIZE + pos[1]].occ_status == -1) val_pos = 1;
	} while(!val_pos);

}


/******************************************************************************
 * init_client()
 *
 * Arguments: client - struct with all the client's info
 * Returns:
 *
 * Description: Initializes the client
 *****************************************************************************/
void get_new_pos(int* pos, int key)
{
	if (key == KEY_UP){
        if (pos[1] != 1){
            pos[1]--;
        }
		printf("KEY_UP\n");
    }
    else if (key == KEY_DOWN){
        if (pos[1] != WINDOW_SIZE-2){
            pos[1]++;
        }
		printf("KEY_DOWN\n");
	}
    else if (key == KEY_LEFT){
        if (pos[0] != 1){
            pos[0]--;
        }
		printf("KEY_LEFT\n");
    }
    else if (key == KEY_RIGHT){
        if (pos[0] != WINDOW_SIZE-2){
            pos[0]++;
		}
		printf("KEY_RIGHT\n");
    }
	
}

void update_user_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx)
{
	if (map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == -1) 
	{	
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = -1;
		field_status->user[idx].pos[0] = new_pos[0];
		field_status->user[idx].pos[1] = new_pos[1];
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status= 0;
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].idx = idx;
	}
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 0 && field_status->user[idx].id != field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].id)
	{
		if (field_status->user[idx].hp < 10){
			field_status->user[idx].hp++;
		}
		field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp--;
		printf("HP LOSS: %d\n", field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp);
		
		if (field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp == 0){
			map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status = -1;
		}
	}
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 2)
	{
		printf("HP GAINED: %d + %d\n", field_status->user[idx].hp, field_status->prize[map[new_pos[0] * WINDOW_SIZE + new_pos[1]].idx].value);
		field_status->user[idx].hp += field_status->prize[map[new_pos[0] * WINDOW_SIZE + new_pos[1]].idx].value;
		
		if (field_status->user[idx].hp > 10){
			field_status->user[idx].hp = 10;
		}

		printf("HP ACTUAL: %d \n", field_status->user[idx].hp);
		
		field_status->prize[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].value = -1;

		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = -1;
		field_status->user[idx].pos[0] = new_pos[0];
		field_status->user[idx].pos[1] = new_pos[1];
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = 0;
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].idx = idx;
	}	
}

void update_bot_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx)
{
	if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == -1) 
	{	
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].occ_status = -1;
		field_status->bot[idx].pos[0] = new_pos[0];
		field_status->bot[idx].pos[1] = new_pos[1];
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].occ_status = 1;
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].idx = idx;
	}
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 0)
	{
		field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp--;
		printf("USER: %c HP LOSS: %d\n", field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].id, field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp);
		
		if (field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp == 0){
			map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status= -1;
		}
	}
}

void init_prize(prize_t* prize)
{
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		prize[i].value = -1;
	}
}

void create_prize(position_t* map, prize_t* prize)
{
	srand(time(0));
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		if(prize[i].value == -1)
		{
			generate_valid_pos(map, prize[i].pos);
			prize[i].value = 1 + random()%5;
			map[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].occ_status = 2;
			map[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].idx = i;
			break;
		}
	}
}

void init_field(position_t* map){
	for (int i = 0; i < WINDOW_SIZE*WINDOW_SIZE; i++){
		map[i].occ_status = -1;
	}
}

int create_user(client_t* user, position_t* map, message_c2s msg){
	int i;
	for(i = 0; i < N_Max_Players; i++){
		if (user[i].id == '-'){
			user[i].id = msg.id;
			user[i].idx = i;
			generate_valid_pos(map, user[i].pos);
			user[i].hp = 10;
			map[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].occ_status = 0;
			map[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].idx = i;
			break;
		}
	}
	return i;

}

void create_bots(client_t* bot, position_t* map, int n_bots){
	for (int i = 0; i < n_bots; i++){
		bot[i].id = '*';
		bot[i].idx = i;
		generate_valid_pos(map, bot[i].pos);
		map[bot[i].pos[0]*WINDOW_SIZE + bot[i].pos[1]].occ_status = 1;
		map[bot[i].pos[0]*WINDOW_SIZE + bot[i].pos[1]].idx = i;
	}
	
}

void clear_user(client_t* user, position_t* map)
{
	map[user->pos[0]*WINDOW_SIZE + user->pos[1]].occ_status = -1;
	user->id = '-';
}


void init_server(field_status_t* field_status, position_t* map){
	init_client(field_status->user);
	init_field(map);
	init_prize(field_status->prize);
	init_client(field_status->bot);

	for (int i = 0; i < 5; i++){
		create_prize(map, field_status->prize);
	}
	
}

int check_ID_val(client_t* client, char id)
{
	for(int i = 0; i < N_Max_Players; i++)
	{
		if(client[i].id == id)
			return 0;
	}
	return 1;
}

void index_mask(field_status_t* field_status){
	for(int i = 0; i < 10; i++)
	{
		field_status->user[i].idx = -1;
		field_status->bot[i].idx = -1;
	}
}

int main(){

	field_status_t field_status;
	position_t map[WINDOW_SIZE*WINDOW_SIZE];
	int sock_fd;
	message_c2s msg_rcv;
	message_s2c msg_send;
	int err;
	int idx = 0;
	int new_pos[2];
	int val_ID;
	client_t invalid_id;
	
	init_server(&field_status, map);


	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		fprintf(stderr, "error: %s\n", strerror(errno)); 
		exit(-1);
	}

	struct sockaddr_un local_addr;
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	unlink(SOCK_ADDRESS);
	err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno)); 
		exit(-1);
	}

	printf(" socket created and binded \n ");
	printf("Ready to receive messages\n");
	
	struct sockaddr_un client_addr;
	socklen_t client_addr_size = sizeof(struct sockaddr_un);
	
	while(1){
		
		printf("Waiting...\n");
		err = recvfrom(sock_fd, &msg_rcv, sizeof(msg_rcv), 0,
		                  ( struct sockaddr *)&client_addr, &client_addr_size);
		if(err == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));            
            exit(0);
        }

		if(msg_rcv.type == Connect){
			printf("OOGA BOOGA IN DA MOOGA\n");

			if(!check_ID_val(field_status.user, msg_rcv.id))
			{
				invalid_id.id = '-';
				err = sendto(sock_fd,
						&invalid_id, sizeof(invalid_id), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
				if(err == -1)
				{
					fprintf(stderr, "error: %s\n", strerror(errno));            
					exit(0);
				}
					
			}
			else
			{
				idx = create_user(field_status.user, map, msg_rcv);
				if(idx == 10)
				{
					invalid_id.id = '/';
					err = sendto(sock_fd,
						&invalid_id, sizeof(invalid_id), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
					if(err == -1)
					{
						fprintf(stderr, "error: %s\n", strerror(errno));            
						exit(0);
					}
				}
				else
				{
					err = sendto(sock_fd,
						&field_status.user[idx], sizeof(field_status.user[idx]), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
					if(err == -1)
					{
						fprintf(stderr, "error: %s\n", strerror(errno));            
						exit(0);
					}
				}
			}
				
	
			
		}
		else if (msg_rcv.type == Connect_bots)
		{
			create_bots(field_status.bot, map, msg_rcv.n_bots);
		}
		else if(msg_rcv.type == Ball_movement && field_status.user[msg_rcv.idx].hp <= 0)
		{
			printf("Health_0\n");
			msg_send.type = Health_0;
			err = sendto(sock_fd,
						&msg_send, sizeof(msg_send), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
			if(err == -1)
			{
				fprintf(stderr, "error: %s\n", strerror(errno));            
				exit(0);
			}
			clear_user(&field_status.user[msg_rcv.idx], map);
		}
		else if(msg_rcv.type == Ball_movement)
		{
			new_pos[0] = field_status.user[msg_rcv.idx].pos[0];
			new_pos[1] = field_status.user[msg_rcv.idx].pos[1];
			printf("Ball Movement\n");
			get_new_pos(new_pos, msg_rcv.key[0]);
			update_user_pos(map, &field_status, new_pos, msg_rcv.idx);
			msg_send.type = Field_status;
			index_mask(&field_status);
			msg_send.field_status = field_status;

			
			err = sendto(sock_fd,
						&msg_send, sizeof(msg_send), 0,
						(const struct sockaddr *) &client_addr, client_addr_size);
			if(err == -1)
			{
				fprintf(stderr, "error: %s\n", strerror(errno));            
				exit(0);
			}

		}
		else if(msg_rcv.type == New_Prize)
		{
			create_prize(map, field_status.prize);
		}
		else if(msg_rcv.type == Bot_movement)
		{
			for(int i = 0; i < N_Max_Bots; i++){
				if(field_status.bot[i].id == '*'){
					new_pos[0] = field_status.bot[i].pos[0];
					new_pos[1] = field_status.bot[i].pos[1];
					printf("Ball Movement\n");
					get_new_pos(new_pos, msg_rcv.key[i]);
					update_bot_pos(map, &field_status, new_pos, i);
				}
				else break;
			}
		}
		else if(msg_rcv.type == Disconnect)
		{
			printf("Disconnect\n");
			clear_user(&field_status.user[msg_rcv.idx], map);
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
