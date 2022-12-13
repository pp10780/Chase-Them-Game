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
        ch = prize->value + '0';
    }else{
        ch = ' ';
    }
    int p_x = prize->pos[0];
    int p_y = prize->pos[1];
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
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
    message_win = newwin(50, 70, WINDOW_SIZE, 0);
    //box(message_win, 0 , 0);	
	wrefresh(message_win);


    message_c2s msg_send;
    message_s2c msg_rcv;
	client_t client;
    client_t personal_info;
    field_status_t field_status;
    field_status_t prev_field_status;

	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

    // Connect
    msg_send.type = Connect;
	msg_send.id = buffer[0];
    nbytes = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));

	nbytes = recv(sock_fd, &client, sizeof(client), 0);

	personal_info = client;
    
    draw_player(my_win, &personal_info, 1);

    //POTENCIAL FONTE DE BUGS
    prev_field_status.user[personal_info.idx] = personal_info;

	while(1){
        key = wgetch(my_win);
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            msg_send.type = Ball_movement;
            msg_send.key = key;
            msg_send.id = personal_info.id;
            msg_send.idx = personal_info.idx;

            nbytes = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
        }
        else if(key == 'q')
        {
            msg_send.type = Disconnect;
            msg_send.idx = personal_info.idx;
            nbytes = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
            break;
        }
		
		nbytes = recv(sock_fd, &msg_rcv, sizeof(msg_rcv), 0);
        
        if(msg_rcv.type == Field_status){
        
            field_status = msg_rcv.field_status;
            for(int i = 0; i < 10; i++)
            {
                if(prev_field_status.user[i].id != '-' && prev_field_status.user[i].hp > 0){      
                    draw_player(my_win, &prev_field_status.user[i], 0);        
                }
                if(field_status.user[i].id != '-' && field_status.user[i].hp > 0){      
                    draw_player(my_win, &field_status.user[i], 1);        
                }
                //Se der bug é porque é como o de cima
                if(field_status.prize[i].value != -1){      
                    draw_prize(my_win, &prev_field_status.prize[i], 0);
                    draw_prize(my_win, &field_status.prize[i], 1);        
                }
            }
            prev_field_status = field_status;
        }

        else if(msg_rcv.type == Health_0){
            
            //touchwin(message_win);
    mvwprintw(message_win, 1,1,
         "\t__  __   ____    __  __                 \n"
         "\t\\ \\/ /  / __ \\  / / / /           \n"
         "\t \\  /  / / / / / / / /            \n"
         "\t / /  / /_/ / / /_/ /             \n"
         "\t/_/   \\____/  \\____/              \n"
         "\t                                 \n"
         "\t                  ____     ____    ______    ____ \n"
         "\t                 / __ \\   /  _/   / ____/   / __ \\ \n"
         "\t                / / / /   / /    / __/     / / / / \n"
         "\t               / /_/ /  _/ /    / /___    / /_/ /  \n"
         "\t              /_____/  /___/   /_____/   /_____/   \n"
         "\t                                                  \n");
         wrefresh(message_win);	
            sleep(3);
            break;
        }
	}

    //endwin();
    //touchwin(message_win);
    mvwprintw(message_win, 1,1,
         "\t   ______    ___     __  ___    ______                   \n"
         "\t  / ____/   /   |   /  |/  /   / ____/                   \n"
         "\t / / __    / /| |  / /|_/ /   / __/                      \n"
         "\t/ /_/ /   / ___ | / /  / /   / /___                      \n"
         "\t\\____/   /_/  |_|/_/  /_/   /_____/                      \n"
         "\t                                                         \n"
         "\t                        ____  _    __    ______    ____  \n"
         "\t                       / __ \\| |  / /   / ____/   / __ \\ \n"
         "\t                      / / / /| | / /   / __/     / /_/ / \n"
         "\t                     / /_/ / | |/ /   / /___    / _, _/  \n"
         "\t                     \\____/  |___/   /_____/   /_/ |_|   \n"
         "\t                                                        \n");
    wrefresh(message_win);	
    sleep(5);
    endwin();

	
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
