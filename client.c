#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pipe_networking.h"

int loginProcedure(int, int);
void interpreter();

//statusNumbers
//1 - enter username (auth)
//2 - enter password (auth)
//3 - confirm password (auth)

int main() {


	int to_server, from_server;
	int loggedIn = 0;
	char username[40];

	from_server = client_handshake( &to_server );

	while(1){
		if(! loggedIn){
			loggedIn = loginProcedure(to_server, from_server);
		}else{
			interpreter();
		}
		//do other stuff needed after login here
	}

	return 0;
}


int loginProcedure(int to_server, int from_server){//working on this, will communicate with server several times
	char buffer[MESSAGE_BUFFER_SIZE];

	//login
	printf("Enter Login: ");
	fgets( &buffer[1], sizeof(buffer) - 1, stdin );
	printf("buffer: %s\n", buffer);
	char *q = &buffer[0];
	*q = '1'; //login statusNumber
	char *p = strchr(buffer, '\n');
	*p = 0;

	printf("%s\n", buffer);
	write( to_server, buffer, sizeof(buffer) );
	read( from_server, buffer, sizeof(buffer) );

	//password
	printf( "(from server) %s\n", buffer );


	printf("%s\n", buffer);
	if (strcmp(buffer, "User already exists in database, enter your password.") == 0){
		//login
		printf("Enter Your Password: ");
		fgets( &buffer[1], sizeof(buffer) - 1, stdin );
		printf("buffer: %s\n", buffer);
		char *q = &buffer[0];
		*q = '2'; //login statusNumber
		char *p = strchr(buffer, '\n');
		*p = 0;
		write( to_server, buffer, sizeof(buffer) );

	}else{
		//register
		printf("Enter New Password: ");
		fgets( &buffer[1], sizeof(buffer) - 1, stdin );
		printf("buffer: %s\n", buffer);
		char *q = &buffer[0];
		*q = '3'; //login statusNumber
		char *p = strchr(buffer, '\n');
		*p = 0;
		write( to_server, buffer, sizeof(buffer) );
	}

	read( from_server, buffer, sizeof(buffer) );
	printf("buffer (from server): %s\n", buffer);
	if (strcmp(buffer, "Login Successful") == 0){
		printf("YOU ARE LOGGED IN!\n");
		return 1;
	}
	return 0;

	//sends login, gets status back
	//either tells user password is needed for normal login
	//or that they will need to create account
	//stops running once logged in
}

void interpreter(){
	printf("Welcome to Fnake\n");
}
