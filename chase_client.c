#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


#include "chase.h"

WINDOW * message_win;


typedef struct player_position_t{
    int x, y;
    char c;
} player_position_t;

void new_player (player_position_t * player, char c){
    player->x = WINDOW_SIZE/2;
    player->y = WINDOW_SIZE/2;
    player->c = c;
}

void draw_player(WINDOW *win, client_t * player, int delete){
    int ch;
    if(delete){
        ch = player->id;
    }else{
        ch = ' ';
    }
    int p_x = player->pos[0];
    int p_y = player->pos[1];
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

void draw_prize(WINDOW *win, prize_t * prize, int delete){
    int ch;
    if(delete){
        ch = prize->value;
    }else{
        ch = ' ';
    }
    int p_x = prize->pos[0];
    int p_y = prize->pos[1];
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

void moove_player (player_position_t * player, int direction){
    if (direction == KEY_UP){
        if (player->y  != 1){
            player->y --;
        }
    }
    if (direction == KEY_DOWN){
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (player->x  != 1){
            player->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (player->x  != WINDOW_SIZE-2){
            player->x ++;
    }
}

int main(){

	char buff[100];
	int nbytes;
    int key = -1;


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

    char buffer[100];
    printf("Choose a letter: ");
    fgets(buffer, 2, stdin);

	initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    //message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    //box(message_win, 0 , 0);	
	//wrefresh(message_win);


    message msg;
	client_t client;
    client_t personal_info;
    field_status_t field;
    field_status_t prev_field;

	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

    // Connect
    msg.type = Connect;
	msg.id = buffer[0];
    nbytes = sendto(sock_fd,
	                    &msg, sizeof(msg), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));

	nbytes = recv(sock_fd, &client, sizeof(client), 0);

	personal_info = client;
    
    draw_player(my_win, &personal_info, 1);

    //POTENCIAL FONTE DE BUGS
    prev_field.user[personal_info.idx] = personal_info;

	while(1){
        key = wgetch(my_win);
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            msg.type = Ball_movement;
            msg.key = key;
            msg.id = personal_info.id;
            msg.idx = personal_info.idx;

            nbytes = sendto(sock_fd,
	                    &msg, sizeof(msg), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
        }
		
		nbytes = recv(sock_fd, &field, sizeof(field), 0);
        
        
        for(int i = 0; i < 10; i++)
        {
            if(field.user[i].id != '-'){      
                draw_player(my_win, &prev_field.user[i], 0);
                draw_player(my_win, &field.user[i], 1);        
            }
        }

        for(int i = 0; i < 10; i++)
        {
            if(field.prize[i].value != -1){      
                draw_prize(my_win, &prev_field.prize[i], 0);
                draw_prize(my_win, &field.prize[i], 1);        
            }
        }
        prev_field = field;
        
	}

	
    close(sock_fd);
	exit(0);
}

// player_position_t p1;

// int main(){
// 	initscr();		    	/* Start curses mode 		*/
// 	cbreak();				/* Line buffering disabled	*/
//     keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
// 	noecho();			    /* Don't echo() while we do getch */

//     /* creates a window and draws a border */
//     WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
//     box(my_win, 0 , 0);	
// 	wrefresh(my_win);
//     keypad(my_win, true);
//     /* creates a window and draws a border */
//     message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
//     box(message_win, 0 , 0);	
// 	wrefresh(message_win);


//     new_player(&p1, 'y');
//     draw_player(my_win, &p1, true);

//     int key = -1;
//     while(key != 27 && key!= 'q'){
//         key = wgetch(my_win);		
//         if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
//             draw_player(my_win, &p1, false);
//             moove_player (&p1, key);
//             draw_player(my_win, &p1, true);

//         }

//         mvwprintw(message_win, 1,1,"%c key pressed", key);
//         wrefresh(message_win);	
//     }

//     exit(0);
// }
