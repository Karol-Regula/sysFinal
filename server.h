#define MAX 4

struct rooms{
	char roomName[100];
	int capacity;
	int ready;
	char player1[100];
	char player2[100];
	char player3[100];
	char player4[100];
	char input;
} rooms;

void sigHandler(int);
void process(char *);
int userExists(char *);
char* authenticate(char *);
int checkPassword(char *);
void sub_server(int);
void addAccount(char *);
char* joinRoom(char *, struct rooms *);
char* createRoom(char *, struct rooms *);
char* leaveRoom(char *, struct rooms *);
char* roomToString(struct rooms *, int);
char* roomsToString(struct rooms *);
char* currentRoomToString(struct rooms *, char *);
void readyPlus(struct rooms *, char *);
char * startGame(struct rooms *, char *);
char* readMove(struct rooms *, char *);
void writeMove(struct rooms *, char *);
int doesExist(char*, struct rooms *);