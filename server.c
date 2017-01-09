#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "pipe_networking.h"

void process( char * s );

int main() {

	int to_client, from_client;
	char buffer[MESSAGE_BUFFER_SIZE];

	to_client = server_handshake( &from_client );

	while(1){
		read( from_client, buffer, sizeof(buffer) );
		//HOW I SEE THIS: Clinent not only sends what user inputs but an extra bit, a number that indicates what the server must do with the input
		//if (statusNumber == 1){authenticate
		authenticate(buffer);
		//if (statusNumber == 2){something else etc.
		write( to_client, buffer, sizeof(buffer));
	}

	return 0;
}


void authenticate( char * s ) { //handles all authentication cases
	if userExists(s){
		//prompt for password
		*s = "User already exists in database, enter your password."
	}
	else{
		//ask for password, password confirmation, and entry (username and password) if registration successful
		*s = "New user, please enter password."
	}
}

void userExists(char * s){
	char buffer[100];
	int fdData = open("database.csv", O_CREAT | O_RDONLY, 0644);//remove creat manually for now
	read(fdData, buffer, sizeof(buffer));
}
