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

int main(int argc, char *argv[]) {

	char *host;
  if (argc != 2 ) {
	 	printf("host not specified, conneting to 127.0.0.1\n");
	 	host = "127.0.0.1";
 	}
 	else
	 	host = argv[1];
 	int sd;

 sd = client_connect( host );


	int loggedIn = 0;
	char username[40];

	while(1){
		if(! loggedIn){
			loggedIn = loginProcedure(sd);
		}else{
			interpreter();
		}
		//do other stuff needed after login here
	}

	return 0;
}


int loginProcedure(int sd){//working on this, will communicate with server several times
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
	write( sd, buffer, sizeof(buffer) );
	read( sd, buffer, sizeof(buffer) );

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
		write( sd, buffer, sizeof(buffer) );

	}else{
		//register
		printf("Enter New Password: ");
		fgets( &buffer[1], sizeof(buffer) - 1, stdin );
		printf("buffer: %s\n", buffer);
		char *q = &buffer[0];
		*q = '3'; //login statusNumber
		char *p = strchr(buffer, '\n');
		*p = 0;
		write( sd, buffer, sizeof(buffer) );
	}

	read( sd, buffer, sizeof(buffer) );
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
