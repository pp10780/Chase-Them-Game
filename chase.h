#define WINDOW_SIZE 20
#define SOCK_ADDRESS "/tmp/sock_16"

#define Connect 1000
#define Ball_information 2000
#define Ball_movement 3000
#define Field_status 4000
#define Health_0 5000
#define Disconnect 6000



typedef struct field_t
{
    // status: -1 - empty; 
    //          0 - player; 
    //          1 - bot; 
    //          2 - prize
    int status;
    // player/bot position on the struct
	int idx;
    // prize: 1 - 5
	int prize;
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

typedef struct message_t
{
    int type;
    int idx;
    char id;
    int key;

} message;
