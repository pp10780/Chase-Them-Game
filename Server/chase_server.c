#include "init_server.h"
#include "movement.h"

int main(){

	field_status_t field_status;
	position_t map[WINDOW_SIZE*WINDOW_SIZE];
	int sock_fd;
	message_c2s msg_rcv;
	message_s2c msg_send;
	int err;
	int idx = 0;
	int new_pos[2];
	client_t invalid_id;
	
	init_server(&field_status, map);

	//open socket for communication
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
			//if id recieved is invalid send message to the client to try again
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
				//if id is valid create a user
				idx = create_user(field_status.user, map, msg_rcv.id);
				//if server full send message to the client to try again later
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
				//if server not full send ball information to the client
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
		//if the health of the client trying to move alredy reached 0 send Health_0 message for it to disconnect and delete the user from the game
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
		//if message recieved Ball movement, process movement and respond with updated field status
		else if(msg_rcv.type == Ball_movement)
		{
			new_pos[0] = field_status.user[msg_rcv.idx].pos[0];
			new_pos[1] = field_status.user[msg_rcv.idx].pos[1];
			printf("Ball Movement\n");
			get_new_pos(new_pos, msg_rcv.key[0]);
			update_user_pos(map, &field_status, new_pos, msg_rcv.idx);
			msg_send.type = Field_status;
			//mask other players indexes to avoid cheating
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

		//if message recieved Bot movement, process movement of all bots in game and update the field status
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
			printf("Invalid message received\n");
		}
	}

	close(sock_fd);
	//unlink(SOCK_ADDRESS);
	exit(0);
}
