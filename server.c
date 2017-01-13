#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "pipe_networking.h"

void process(char *);
int userExists(char *);
void authenticate(char *);

int main() {

	int to_client, from_client;
	char * buffer[MESSAGE_BUFFER_SIZE];

	to_client = server_handshake( &from_client );

	while(1){
		read( from_client, buffer, sizeof(buffer) );
		//HOW I SEE THIS: Clinent not only sends what user inputs but an extra bit, a number that indicates what the server must do with the input
		//if (statusNumber == 1){authenticate
		authenticate(*buffer);
		//if (statusNumber == 2){something else etc.
		write( to_client, buffer, sizeof(buffer));
	}

	return 0;
}


void authenticate(char * s) { //handles all authentication cases
	printf("authenticating\n");
	if (userExists(s)){
		//prompt for password
		s = "User already exists in database, enter your password.";
	}
	else{
		//ask for password, password confirmation, and entry (username and password) if registration successful
		s = "New user, please enter password.";
	}
}

int userExists(char * s){
	printf("checking if user already exists\n");
	//reads line by line and compares usernames

	char * buffer = NULL;
	int size;
	size_t len = 2;
	int fdData;
	printf("opening database:\n");
	fdData = open("database.csv", O_CREAT | O_RDONLY, 0644);//remove creat manually for now

	printf("reading database:\n");
	read(fdData, buffer, len);
	printf("%s", buffer);

	return 1; //for now
}
