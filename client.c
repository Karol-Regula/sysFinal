#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pipe_networking.h"

int loginProcedure(int, char**);
void interpreter(int, char*);
char** parseHelper(char*, char*);
void welcomePrint();
void helpPrint();
void lobbyPrint(char*);
void joinedPrint(char*);

//statusNumbers
//1 - enter username (auth)
//2 - enter password (auth)
//3 - confirm password (auth)

//Also have/do:
//test on school computers
//clone fresh and test
//add to period 5 repo
//readme file - listed on site, names of group members and periods (one repo per group?)
//  -purpose of thing, instructions for running, how to use (tutorial)
//  -bugs and concerns
//

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
	char* username;
	while(1){
		if(!(loggedIn)){
			loggedIn = loginProcedure(sd, &username);
		}
		else{
			interpreter(sd, username);
			exit(1);
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

void helpPrint(){
	//system("clear");
	printf("[CLIENT] " ANSI_COLOR_GREEN "Fnake Help Utility!" ANSI_COLOR_RESET "\n");
	printf("[CLIENT] Available commands: \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!help" ANSI_COLOR_RESET " - opens the help utility\n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!exit" ANSI_COLOR_RESET " - exit Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!join" ANSI_COLOR_RESET " - join a game of Fnake \n");
	printf("[CLIENT] " ANSI_COLOR_YELLOWER "!refresh" ANSI_COLOR_RESET " - refreshes list of rooms \n");
	printf("---------------------------------------\n");
	//printf("!game - joins game \n");
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
		else if (pNum == atoi(room[1])) //room is full
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
	printf("You have joined "ANSI_COLOR_CYAN"%s"ANSI_COLOR_RESET" !\n", room[0]);
	int pos = 3;
	printf("Users currently in the room:\n");
	while(room[pos]){
	 printf("\t%s\n", room[pos]);
	 pos++;
	}
	free(room);
}

void interpreter(int sd, char* username){
	welcome();
	char buffer[MESSAGE_BUFFER_SIZE];
	while (1){
		printf("[CLIENT] @ (%s): ", username);
		fgets( &buffer[1], sizeof(buffer) - 1, stdin);
		*(strchr(buffer, '\n')) = 0;
		//printf("%d\n", strcmp(&buffer[1], "!help"));

		if (strcmp(&buffer[1], "!help") == 0){
			helpPrint();
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
			printf("buffer after read refresh: %s\n", buffer);
			//lobbyPrint(buffer);
		}
		else if (strncmp(&buffer[1], "!join", 5) == 0){
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
			}
		}
		else if (strncmp(&buffer[1], "!create", 6) == 0){
			char *statusNum = &buffer[0];
			*statusNum = '5';
			strcat(buffer, " ");
			strcat(buffer, username);
			printf("(debug) sending buffer for !create: %s\n", buffer);
			write(sd, buffer, sizeof(buffer));
			read(sd, buffer, sizeof(buffer));
			printf("(debug) returned buffer for !create: %s\n", buffer);
			if (strcmp(buffer, "success") == 0)
				printf("[CLIENT] Room created successfully!\n");
		}
		else{
			printf ("[CLIENT] Unknown command. Enter "ANSI_COLOR_YELLOW"!help"ANSI_COLOR_RESET" to display help menu.\n");
		}
	}
}
