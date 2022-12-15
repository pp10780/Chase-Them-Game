OBJS	= init_server.o movement.o utils_server.o chase_server.o
SOURCE	= init_server.c movement.c utils_server.c chase_server.c
HEADER	= defines.h init_server.h movement.h structs.h utils_server.h
OUT	= server
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

init_server.o: init_server.c
	$(CC) $(FLAGS) init_server.c 

movement.o: movement.c
	$(CC) $(FLAGS) movement.c 

utils_server.o: utils_server.c
	$(CC) $(FLAGS) utils_server.c 

chase_server.o: chase_server.c
	$(CC) $(FLAGS) chase_server.c 


clean:
	rm -f $(OBJS) $(OUT)
