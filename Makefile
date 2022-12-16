OBJDIR = obj
OUTDIR = bin

OBJS = chase_server.o init_server.o movement.o utils_server.o chase_client.o chase_bots.o chase_timer.o
OUT	= server, client, bots, timer

OBJS0	= $(OBJDIR)/chase_server.o $(OBJDIR)/init_server.o $(OBJDIR)/movement.o $(OBJDIR)/utils_server.o
SOURCE0	= chase_server.c init_server.c movement.c utils_server.c
HEADER0	= defines.h init_server.h movement.h structs.h utils_server.h
OUT0	= $(OUTDIR)/server
SRCDIR0 = Server

OBJS1	= $(OBJDIR)/chase_client.o
SOURCE1	= chase_client.c
HEADER1	= defines.h structs.h
OUT1	= $(OUTDIR)/client
SRCDIR1 = Player_client

OBJS2	= $(OBJDIR)/chase_bots.o
SOURCE2	= chase_bots.c
HEADER2	= defines.h structs.h
OUT2	= $(OUTDIR)/bots
SRCDIR2 = Bot_client

OBJS3	= $(OBJDIR)/chase_timer.o
SOURCE3	= chase_timer.c
HEADER3	= defines.h structs.h
OUT3	= $(OUTDIR)/timer
SRCDIR3 = Timer

CC	 = gcc
FLAGS	 = -g -c -Wall


all: server client bots timer

server: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0)

client: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1) -lncurses

bots: $(OBJS2) $(LFLAGS)
	$(CC) -g $(OBJS2) -o $(OUT2)

timer: $(OBJS3) $(LFLAGS)
	$(CC) -g $(OBJS3) -o $(OUT3)

$(OBJDIR)/chase_server.o: $(SRCDIR0)/chase_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/chase_server.c -o $(OBJDIR)/chase_server.o

$(OBJDIR)/init_server.o: $(SRCDIR0)/init_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/init_server.c -o $(OBJDIR)/init_server.o

$(OBJDIR)/movement.o: $(SRCDIR0)/movement.c
	$(CC) $(FLAGS) $(SRCDIR0)/movement.c -o $(OBJDIR)/movement.o

$(OBJDIR)/utils_server.o: $(SRCDIR0)/utils_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/utils_server.c -o $(OBJDIR)/utils_server.o

$(OBJDIR)/chase_client.o: $(SRCDIR1)/chase_client.c
	$(CC) $(FLAGS) $(SRCDIR1)/chase_client.c -o $(OBJDIR)/chase_client.o

$(OBJDIR)/chase_bots.o: $(SRCDIR2)/chase_bots.c
	$(CC) $(FLAGS) $(SRCDIR2)/chase_bots.c -o $(OBJDIR)/chase_bots.o

$(OBJDIR)/chase_timer.o: $(SRCDIR3)/chase_timer.c
	$(CC) $(FLAGS) $(SRCDIR3)/chase_timer.c -o $(OBJDIR)/chase_timer.o


clean:
	rm -f $(OBJDIR)/*.o $(OUTDIR)/*