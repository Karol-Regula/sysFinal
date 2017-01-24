#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <assert.h>
#include <poll.h>

#include "fnake.h"
#include "pipe_networking.h"

int loginProcedure(int, char**);
int lobbyInterpreter(int, char*, char **);
int roomInterpreter(int, char*, char*, int*, int*, int*);
void gameInterpreter(int, char*, char*, int*, int*);
char** parseHelper(char*, char*);
void welcomePrint();
void helpPrintLobby();
void helpPrintRoom();
void lobbyPrint(char*);
void joinedPrint(char*);

//statusNumbers
//1 - enter username (auth)
//2 - enter password (auth)
//3 - confirm password (auth)
//4 - join room (lobby)
//5 - create room (lobby)
//6 - refresh (lobby)
//7 - list players (room)
//8 - leave room (room)
//9 - ready (room)

//Also have/do:
//test on school computers
//clone fresh and test
//add to period 5 repo
//readme file - listed on site, names of group members and periods (one repo per group?)
//  -purpose of thing, instructions for running, how to use (tutorial)
//  -bugs and concerns

//sending data to multiple clients - google this (single port might help)
//what is a select server

int main(int argc, char *argv[]){
	printf("[CLIENT] booting...\n");
	char *host;
	if (argc != 2){
	 	printf("[CLIENT] host not specified, conneting to 127.0.0.1\n");
	 	host = "127.0.0.1";
 	}
 	else host = argv[1];
 	int sd;
	sd = client_connect(host);
	int loggedIn = 0;
	int inRoom = 0;
	int isReady = 0;
	int inGame = 0;
	int userNumber = -1;
	char* username;
	char* roomname;
	while(1){
		if(!(loggedIn)){
			loggedIn = loginProcedure(sd, &username);
		}
		else if(!(inRoom)){
			inRoom = lobbyInterpreter(sd, username, &roomname);
		}
		else if(!(inGame)){
			inRoom = roomInterpreter(sd, username, roomname, &isReady, &inGame, &userNumber);
		}
		else{
			gameInterpreter(sd, username, roomname, &userNumber, &inGame);
		}
	}
	return 0;
}


int loginProcedure(int sd, char** username){//working on this, will communicate with server several times
	char buffer[MESSAGE_BUFFER_SIZE];
	char bufferUsername[MESSAGE_BUFFER_SIZE];

	//username
	printf("[CLIENT] Enter Username: ");
	fgets( &buffer[1], sizeof(buffer) - 1, stdin );
	strcpy(bufferUsername, &buffer[1]);
	*username = bufferUsername;
	*(strchr(bufferUsername, '\n')) = 0;
	char *q = &buffer[0];
	*q = '1'; //login statusNumber
	char *p = strchr(buffer, '\n');
	*p = 0;

	printf("(debug) buffer string: %s\n", buffer);
	write(sd, buffer, sizeof(buffer));
	read(sd, buffer, sizeof(buffer));
	printf("(debug) Server says: %s\n", buffer);

	//login
	if (strcmp(buffer, "user exists") == 0){
		printf("[CLIENT] Welcome back! Please enter your username.\n");
		printf("[CLIENT] Enter Your Password: ");
		fgets( &buffer[1], sizeof(buffer) - 1, stdin );
		printf("buffer: %s\n", buffer);
		char *q = &buffer[0];
		*q = '2'; //login statusNumber
		char *p = strchr(buffer, '\n');
		*p = 0;
		write(sd, buffer, sizeof(buffer));
	}

	//register
	else{
		char bufferB[MESSAGE_BUFFER_SIZE];
		char userBuffer[MESSAGE_BUFFER_SIZE];
		strcpy(userBuffer, &buffer[1]);
		printf("[CLIENT] Hello " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "! Please enter a password to create your new account.\n", userBuffer);
		int match = 0;
		while (match == 0){
			printf("[CLIENT] Enter New Password: ");
			fgets( &buffer[1], sizeof(buffer) - 1, stdin );
			printf("(debug) bufferA: %s\n", buffer);
			char *p = &buffer[0];
			*p = '3';
			*(strchr(buffer, '\n')) = 0;
			printf("(debug) bufferA again: %s\n", buffer);
			printf("[CLIENT] Re-Enter New Password: ");
			fgets( &bufferB[1], sizeof(buffer) - 1, stdin );
			printf("(debug) bufferB: %s\n", bufferB);
			char *b = &bufferB[0];
			*b = '3';
			*(strchr(bufferB, '\n')) = 0;
			printf("(debug) bufferB again: %s\n", bufferB);
			if (strcmp(buffer, bufferB) == 0){
				printf("[CLIENT] Passwords match!\n");
				match = 1;
			}
			else printf("[CLIENT] Passwords " ANSI_COLOR_RED "do not match!\n" ANSI_COLOR_RESET);
		}
		strcat(buffer, "?");
		strcat(buffer, userBuffer);
		printf("(debug) buffer to send to server: %s\n", buffer);
		write(sd, buffer, sizeof(buffer));
	}

	read(sd, buffer, sizeof(buffer));
	printf("buffer (from server): %s\n", buffer);
	if (strcmp(buffer, "Login successful") == 0){
		printf("[CLIENT] You are now " ANSI_COLOR_GREEN "logged in!" ANSI_COLOR_RESET "\n");
		return 1;
	}
	if (strcmp(buffer, "Login failed") == 0)
		printf("[CLIENT] Username and password " ANSI_COLOR_RED "do not match" ANSI_COLOR_RESET " -- please try again.\n");
	return 0;

	//sends login, gets status back
	//either tells user password is needed for normal login
	//or that they will need to create account
	//stops running once logged in
}

char** parseHelper(char* input, char* delim){
	if (input == NULL) return NULL;
	char** args = (char**)malloc(sizeof(char**) * 100);
	int i = 0;
	char* dup = strdup(input);
	while (dup){
		args[i] = strsep(&dup, delim);
		i++;
	}
	args[i] = 0;
	free(dup);
	return args;
}

void welcome(){
	//system("clear");
	printf("[CLIENT] " ANSI_COLOR_GREEN "Welcome to Fnake!" ANSI_COLOR_RESET "\n");
	printf("             ____\n            / . .\\\n            \\  ---<\n             \\  /\n   __________/ /\n-=:___________/\n\n\n");
	printf("[CLIENT] Enter a command using !<command>\n");
  printf("[CLIENT] Type \"!help\" for a list of commands\n");
}

void helpPrintLobby(){
	printf("[CLIENT] " ANSI_COLOR_GREEN "Fnake Help Utility!" ANSI_COLOR_RESET "\n");
	printf("[CLIENT] Available commands: \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!help" ANSI_COLOR_RESET " - opens the help utility\n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!exit" ANSI_COLOR_RESET " - exit Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!join <roomName>" ANSI_COLOR_RESET " - join a game of Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!create <roomName>" ANSI_COLOR_RESET " - create a game of Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!refresh" ANSI_COLOR_RESET " - refreshes list of rooms \n");
	printf("---------------------------------------\n");
}

void helpPrintRoom(){
	printf("[CLIENT] " ANSI_COLOR_GREEN "Fnake Help Utility!" ANSI_COLOR_RESET "\n");
	printf("[CLIENT] Available commands: \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!help" ANSI_COLOR_RESET " - opens the help utility\n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!exit" ANSI_COLOR_RESET " - exit Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!leave" ANSI_COLOR_RESET " - leaves your current room \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!refresh" ANSI_COLOR_RESET " - refreshes list of players in room \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!ready" ANSI_COLOR_RESET " - indicates that you are ready to play \n");
	printf("---------------------------------------\n");
}

void lobbyPrint(char* data){
	char** roomAll = (char**)malloc(sizeof(char**) * 100);
	roomAll = parseHelper(data, "?");
	int roomNum = 0;
	char** room = (char**)malloc(sizeof(char**) * 100);
	while (roomAll[roomNum]){
		int pNum = 0; //number of users in a room
		room = parseHelper(roomAll[roomNum], " ");
		while (room[pNum]) pNum++;
		pNum -= 3;

		if (pNum == atoi(room[2])) //game is in-session
			printf(ANSI_COLOR_RED"%s "ANSI_COLOR_RESET"(%d in, %s ready, %s max)\n", room[0], pNum, room[2], room[1]);
		else if (pNum == atoi(room[1]) && pNum != 0) //room is full
			printf(ANSI_COLOR_YELLOW"%s "ANSI_COLOR_RESET"(%d in, %s ready, %s max)\n", room[0], pNum, room[2], room[1]);
		else //room is open
			printf(ANSI_COLOR_GREEN"%s "ANSI_COLOR_RESET"(%d in, %s ready, %s max)\n", room[0], pNum, room[2], room[1]);
		roomNum++;
	}
	free(roomAll);
	free(room);
}

void joinedPrint(char* data){
	char** room = (char**)malloc(sizeof(char**) * 100);
	room = parseHelper(data, " ");
	printf("You are in "ANSI_COLOR_CYAN"%s"ANSI_COLOR_RESET"!\n", room[0]);
	int pos = 3;
	printf("Users currently in the room:\n");
	while(room[pos]){
	 printf("\t%s\n", room[pos]);
	 pos++;
	}
	if (atoi(room[2]) == 1){
		printf("1 player is ready.\n");
	}else{
		printf("%s players are ready.\n", room[2]);
	}
	free(room);
}

int lobbyInterpreter(int sd, char* username, char ** roomname){
	welcome();
	char buffer[MESSAGE_BUFFER_SIZE];
	while (1){
		printf("[CLIENT] @ %s: ", username);
		fgets( &buffer[1], sizeof(buffer) - 1, stdin);
		*(strchr(buffer, '\n')) = 0;
		//printf("%d\n", strcmp(&buffer[1], "!help"));

		if (strcmp(&buffer[1], "!help") == 0){
			helpPrintLobby();
		}
		else if (strcmp(&buffer[1], "!exit") == 0){
			printf("[CLIENT] Exiting game..."ANSI_COLOR_CYAN" Thank you "ANSI_COLOR_RESET"for playing!""\n");
			exit(0);
		}
		else if (strcmp(&buffer[1], "!refresh") == 0){
			printf("[CLIENT] Refreshing the lobby...\n");
			strcpy(buffer, "6");
			write(sd, buffer, sizeof(buffer));
			read(sd, buffer, sizeof(buffer));
			printf("lobbyInterpreter(): buffer after read refresh: %s\n", buffer);
			if (strcmp(buffer, "DNE") == 0)
				printf("No rooms currently exist. Make one with the "ANSI_COLOR_YELLOW"!create"ANSI_COLOR_RESET" command.\n");
			else lobbyPrint(buffer);
		}
		else if (strncmp(&buffer[1], "!join", 5) == 0){
			strcpy(*roomname, &(strrchr(buffer, ' '))[1]); //sets roomname just in case
			char *statusNum = &buffer[0];
			*statusNum = '4';
			strcat(buffer, " ");
			strcat(buffer, username);
			write(sd, buffer, sizeof(buffer));
			read(sd, buffer, sizeof(buffer));
			printf("(debug) returned buffer for !join: %s\n", buffer);
			if (strcmp(buffer, "DNE") == 0)
				printf("[CLIENT] Room does not exist, please try a different room. Enter "ANSI_COLOR_YELLOW"!refresh"ANSI_COLOR_RESET" for a list of rooms.\n");
			else if (strcmp(buffer, "full/in") == 0)
				printf("[CLIENT] Room is either full or in-session. Please try another room.\n");
			else{
				joinedPrint(buffer);
				return 1;
			}
		}
		else if (strncmp(&buffer[1], "!create", 6) == 0){
			strcpy(*roomname, &(strrchr(buffer, ' '))[1]); //sets roomname just in case
			char *statusNum = &buffer[0];
			*statusNum = '5';
			printf("here\n");
			strcat(buffer, " ");
			strcat(buffer, username);
			printf("here\n");
			printf("(debug) sending buffer for !create: %s\n", buffer);
			write(sd, buffer, sizeof(buffer));
			read(sd, buffer, sizeof(buffer));
			printf("(debug) returned buffer for !create: %s\n", buffer);
			if (strcmp(buffer, "success") == 0)
				printf("[CLIENT] Room created successfully!\n");
				return 1;
		}
		else{
			printf ("[CLIENT] Unknown command. Enter "ANSI_COLOR_YELLOW"!help"ANSI_COLOR_RESET" to display help menu.\n");
		}
	}
	return 0;
}

int roomInterpreter(int sd, char * username, char* roomname, int* isReady, int* inGame, int* userNumber){
	char buffer[MESSAGE_BUFFER_SIZE];
	
	while(! *inGame){
		printf("[CLIENT] @ %s -- %s: ", username, roomname);
		if (! *isReady){
			fgets( &buffer[1], sizeof(buffer) - 1, stdin);
		  *(strchr(buffer, '\n')) = 0;
		}
		else{
			 int started = 0;
			 while (! started){
			 	sleep(2);
				
			  strcpy(buffer, "0");
				strcat(buffer, " ");
				strcat(buffer, roomname);
				strcat(buffer, " ");
				strcat(buffer, username);
				printf("username: %s\n", username);
				printf("(debug) ready, not started -- buffer to server: %s\n", buffer);
				write(sd, buffer, sizeof(buffer));
				read(sd, buffer, sizeof(buffer));
				printf("(debug) ready, not started yet -- buffer from server: %s\n", buffer);
				if (strcmp(buffer, "-1") != 0){ //user assigned player number, game started
					started = 1;
					*userNumber = atoi(buffer);
					*inGame = 1;
				}
			}
		}
		
		
		if (strcmp(&buffer[1], "!help") == 0){
			helpPrintRoom();
		}
		else if (strcmp(&buffer[1], "!exit") == 0){
			printf("[CLIENT] Exiting game..."ANSI_COLOR_CYAN" Thank you "ANSI_COLOR_RESET"for playing!""\n");
			exit(0);
		}
		else if (strcmp(&buffer[1], "!leave") == 0){
			if (*isReady){
				printf("[CLIENT] Wait for the game to begin.\n");
			}else{
				printf("[CLIENT] Exiting "ANSI_COLOR_CYAN" %s "ANSI_COLOR_RESET"...\n", roomname);
				printf("[CLIENT] call refresh command at this point\n");
				char *statusNum = &buffer[0];
				*statusNum = '8';
					strcat(buffer, " ");
				strcat(buffer, roomname);
				strcat(buffer, " ");
				strcat(buffer, username);
				printf("(debug) room !leave -- buffer to server: %s\n", buffer);
				write(sd, buffer, sizeof(buffer));
				read(sd, buffer, sizeof(buffer));
				printf("(debug) room !leave -- buffer from server: %s\n", buffer);
				*isReady = 0;
				return 0;
			}
		}
		else if (strcmp(&buffer[1], "!refresh") == 0){
			printf("[CLIENT] Refreshing the room...\n");
			strcpy(buffer, "7");
			strcat(buffer, " ");
			strcat(buffer, roomname);
			write(sd, buffer, sizeof(buffer));
			read(sd, buffer, sizeof(buffer));
			printf("roomInterpreter(): buffer after read refresh: %s\n", buffer);
			//printf("username: %s\n", username);
			joinedPrint(buffer);
			//printf("username: %s\n", username);
		}
		else if (strcmp(&buffer[1], "!ready") == 0){
			if (*isReady){
				printf("You have already set your status to ready.\n");
			}
			else{
				strcpy(buffer, "9");
				strcat(buffer, " ");
				strcat(buffer, roomname);
				write(sd, buffer, sizeof(buffer));
				read(sd, buffer, sizeof(buffer));
				printf("buffer after read refresh: %s\n", buffer);
				printf("You are now ready!\n");
				*isReady = 1;
			}
		}
		else{
			printf ("[CLIENT] Unknown command. Enter "ANSI_COLOR_YELLOW"!help"ANSI_COLOR_RESET" to display help menu.\n");
		}
	}
}
	
	
void gameInterpreter(int sd, char* username, char* roomname, int* userNumber, int *inGame){
	char buffer[MESSAGE_BUFFER_SIZE];
	int turn = 1;
	
	int c = 0;
  struct termios org_opts, new_opts;
  int res = 0;
    //-----  stores old settings -----------
  res=tcgetattr(STDIN_FILENO, &org_opts);
  assert(res==0);
      //---- sets new terminal parms --------
  printf("Press a key.\n");
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c = getchar();

  int height = Height;
  char lastInput = 'd'; //'w' up, 'a' left, 's' down, 'd' right
  int width = Width;
  int grid[width][height];
  int Xc = 3;
  int Yc = 3;
  int snakeLength = 6;
  char input;
  char output;
  initGrid(width, height, grid);
  printGrid(width, height, grid);
  placeFood(grid);
  
 	while (! *inGame){
		if (*userNumber == turn){ //send move to server
			//sleep(1);
			input = getInput();
			struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };


			printf("YOUR turn\n");
    	if(poll(&mypoll, 1, 30)){
        scanf("%c", &input);
        printf("Read string - %c\n", input);
    	}
    	else {
        input = lastInput;
    	}
			
			if (input != '\n'){
  	    printf("input: %c\n", input);
    	 	printf("-----------------------------------\n");
    		int dir;
	    	strcpy(buffer, "i");
  	   	strcat(buffer, " ");
    	 	strcat(buffer, &input);
    	 	strcat(buffer, " ");
    	 	strcat(buffer, roomname);
    		write(sd, buffer, sizeof(buffer));
     		printf("sent buffer to server: %s\n", buffer);
			}
		}
		
		else{
			sleep(3);
		}
		sleep(2);
		strcpy(buffer, "o");
		strcat(buffer," ");
		strcat(buffer, roomname);
		write(sd, buffer, sizeof(buffer)); //ask for the most recent move
		read(sd, buffer, sizeof(buffer)); //get the next direction	
		printf("receiving move from server: %s\n", buffer);
		output = buffer[2];
		printf("buffer[2]: %c", buffer[2]);
	  if (output == 'a') Xc -= 1;
    if (output == 'd') Xc += 1;
    if (output == 'w') Yc -= 1;
    if (output == 's') Yc += 1;
    int edit = editGrid(Xc, Yc, grid, snakeLength);
    if (edit == -1){
    	*inGame = 0;
    }
    else{
    	snakeLength = edit;
    }
    cycleGrid(width, height, grid);
    printGrid(width, height, grid);
    turn++;
    if (turn == 5) turn = 1;
  	
 	}
}


