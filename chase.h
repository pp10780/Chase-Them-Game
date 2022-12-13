#define WINDOW_SIZE 20
#define SOCK_ADDRESS "/tmp/sock_16"

#define Connect 1000
#define Ball_information 2000
#define Ball_movement 3000
#define Field_status 4000
#define Health_0 5000
#define New_Prize 6000
#define Bots 7000
#define Disconnect 8000



typedef struct field_t
{
    // status: -1 - empty; 
    //          0 - player; 
    //          1 - bot; 
    //          2 - prize
    int status;
    // player/bot/prize position on the struct
	int idx;
} field_t;

typedef struct prize_t
{
    int value;
    int pos[2];
} prize_t;


typedef struct client_t
{
	char id;
    int idx;
	int hp;
    int pos[2];
} client_t;

typedef struct field_status_t
{
    client_t user[10];
    prize_t prize[10];
}field_status_t;


//Dont send user idx
typedef struct message_s2c
{
    int type;
    field_status_t field_status;
}message_s2c;

typedef struct message_c2s
{
    int type;
    int idx;
    char id;
    int key;
} message_c2s;
