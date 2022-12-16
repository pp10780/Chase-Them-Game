#ifndef INIT_H
#define INIT_H
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

#include "../common/defines.h"
#include "../common/structs.h"
#include "utils_server.h"

void init_server(field_status_t* field_status, position_t* map);
void init_client(client_t* client);
void init_prize(prize_t* prize);
void init_field(position_t* map);

int create_user(client_t* user, position_t* map, char id);
void create_bots(client_t* bot, position_t* map, int n_bots);
void create_prize(position_t* map, prize_t* prize);

#endif