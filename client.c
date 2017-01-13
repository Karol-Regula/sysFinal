#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pipe_networking.h"

int loginProcedure(int, int);

int main() {


	int to_server, from_server;
	int loggedIn = 0;
	char username[40];

	from_server = client_handshake( &to_server );

	while(1){
		if(! loggedIn){
			loginProcedure(to_server, from_server);
		}
		//do other stuff needed after login here
	}

	return 0;
}


int loginProcedure(int to_server, int from_server){//working on this, will communicate with server several times
	char buffer[MESSAGE_BUFFER_SIZE];
	printf("Enter Login: ");
	fgets( &buffer[1], sizeof(buffer), stdin );
	buffer[0] == 0; //login
	char *p = strchr(buffer, '\n');
	*p = 0;

	write( to_server, buffer, sizeof(buffer) );
	read( from_server, buffer, sizeof(buffer) );
	printf( "received: %s\n", buffer );
	//sends login, gets status back
	//either tells user password is needed for normal login
	//or that they will need to create account
	//stops running once logged in
}
