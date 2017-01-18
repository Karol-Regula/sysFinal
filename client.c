#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pipe_networking.h"

int loginProcedure(int);
void interpreter();

//statusNumbers
//1 - enter username (auth)
//2 - enter password (auth)
//3 - confirm password (auth)

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
	char username[40];
	while(1){
		if(!(loggedIn))
			loggedIn = loginProcedure(sd);
		else{
			interpreter();
			exit(1);
		}
	}
	return 0;
}


int loginProcedure(int sd){//working on this, will communicate with server several times
	char buffer[MESSAGE_BUFFER_SIZE];

	//username
	printf("[CLIENT] Enter Username: ");
	fgets( &buffer[1], sizeof(buffer) - 1, stdin );
	char *q = &buffer[0];
	*q = '1'; //login statusNumber
	char *p = strchr(buffer, '\n');
	*p = 0;

	//printf("(debug) buffer string: %s\n", buffer);
	write(sd, buffer, sizeof(buffer));
	read(sd, buffer, sizeof(buffer));
	//printf("(debug) Server says: %s\n", buffer);

	//login
	if (strcmp(buffer, "user exists") == 0){
		printf("[CLIENT] Welcome back! Please enter your username.\n");
		printf("[CLIENT] Enter Your Password: ");
		fgets( &buffer[1], sizeof(buffer) - 1, stdin );
		//printf("buffer: %s\n", buffer);
		char *q = &buffer[0];
		*q = '2'; //login statusNumber
		char *p = strchr(buffer, '\n');
		*p = 0;
		write(sd, buffer, sizeof(buffer));

	}

	//register
	else{
		char bufferB[MESSAGE_BUFFER_SIZE];
		char username[MESSAGE_BUFFER_SIZE];
		strcpy(username, &buffer[1]);
		printf("[CLIENT] Hello " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "! Please enter a password to create your new account.\n", username);
		int match = 0;
		while (match == 0){
			printf("[CLIENT] Enter New Password: ");
			fgets( &buffer[1], sizeof(buffer) - 1, stdin );
			//printf("(debug) bufferA: %s\n", buffer);
			char *p = &buffer[0];
			*p = '3';
			*(strchr(buffer, '\n')) = 0;
			//printf("(debug) bufferA again: %s\n", buffer);
			printf("[CLIENT] Re-Enter New Password: ");
			fgets( &bufferB[1], sizeof(buffer) - 1, stdin );
			//printf("(debug) bufferB: %s\n", bufferB);
			char *b = &bufferB[0];
			*b = '3';
			*(strchr(bufferB, '\n')) = 0;
			//printf("(debug) bufferB again: %s\n", bufferB);

			if (strcmp(buffer, bufferB) == 0){
				printf("[CLIENT] Passwords match!\n");
				match = 1;
			}
			else printf("[CLIENT] Passwords do not match!\n");
		}
		strcat(buffer, "?");
		strcat(buffer, username);
		//printf("(debug) final buffer: %s\n", buffer);
		write(sd, buffer, sizeof(buffer));
	}

	read(sd, buffer, sizeof(buffer));
	//printf("buffer (from server): %s\n", buffer);
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

void interpreter(){
	printf("[CLIENT] Welcome to Fnake!\n");
}
