#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "pipe_networking.h"

void process(char *);
int userExists(char *);
char * authenticate(char *);
int checkPassword(char *);

int main() {

	int to_client, from_client;
	char buffer [MESSAGE_BUFFER_SIZE];
	int statusNumber = -1;

	to_client = server_handshake( &from_client );


	while(1){
		read( from_client, buffer, sizeof(buffer) );
		printf("Getting statusNumber\n");
		statusNumber = atoi(&buffer[0]);
		printf("statusNumber: %d\n", statusNumber);
		printf("buffer: %s\n", buffer);
		//HOW I SEE THIS: Client not only sends what user inputs but an extra bit, a number that indicates what the server must do with the input
		if (statusNumber == 1){
			strcpy(buffer, authenticate(buffer));
		}
		if (statusNumber == 2){
			int out = checkPassword(buffer);
			if (out){
				printf("SERVERS\n");
				strcpy(buffer, "Login Successful");
			}else{
				printf("SERVERF\n");
				strcpy(buffer, "Login Failed");
			}

		}

		printf("buffer (sending back to client): %s\n", buffer);
		write( to_client, buffer, sizeof(buffer));
	}

	return 0;
}


char * authenticate(char * s) { //handles all authentication cases
	printf("authenticating\n");
	if (userExists(s)){
		//prompt for password
		return "User already exists in database, enter your password.";
	}
	else{
		//ask for password, password confirmation, and entry (username and password) if registration successful
		return "New user, please enter password.";
	}
}

int checkPassword(char * s){
	printf("checking if password is correct\n");
	//reads line by line and compares usernames

	int size;
	int soFar = 0;
	struct stat st;
	stat("database.csv", &st);
	size = st.st_size;
	char buffer[size + 1];
	int fdData;
	int offset;
	int temp = 0;



	printf("opening database:\n");
	fdData = open("database.csv", O_RDONLY, 0644);//remove creat manually for now
	printf("size: %d\n", size);

	printf("reading database:\n");
	read(fdData, buffer, size);
	char *p = strchr(buffer, '\n');
	*p = 0;
	offset = (int)(p - buffer);
	printf("offset: %d\n", offset);
	printf("first buffer: %s\n", buffer);
	printf("sizeof(buffer) %lu\n", sizeof(buffer));
	soFar += (offset + 1);
	size -= (offset + 1);
	printf("soFar: %d\n", soFar);
	printf("size: %d\n", size);
	lseek(fdData, soFar, SEEK_SET);

	while(size){
		printf("here1\n");
		read(fdData, buffer, size);
		buffer[size] = 0;
		printf("buffer: %s\n", buffer);
		printf("here2\n");
		char *p = strchr(buffer, '\n');
		printf("here3\n");
		printf("%s\n", p);
		*p = 0;
		printf("here4\n");
		offset = (int)(p - buffer);
		printf("here5\n");
		soFar += (offset + 1);
		printf("here6\n");
		size -= (offset + 1);
		printf("here7\n");
		printf("post buffer: %s\n", buffer);

		char *r =  strchr(buffer, ',');
		printf("here9\n");
		temp = (int)((r - buffer) + 1);
		printf("temp: %d\n", temp);

		printf("buffer (password only): %s\n", &buffer[temp]);
		printf("from client: %s\n", &s[1]);
		if (strcmp(&s[1], &buffer[temp]) == 0){
			printf("password matches!\n");
			return 1;
		}

		printf("soFar: %d\n", soFar);
		printf("size: %d\n", size);
		lseek(fdData, soFar, SEEK_SET);


	}
	printf("password does not match!\n");
	return 0; //for now
}

int userExists(char * s){
	printf("checking if user already exists\n");
	//reads line by line and compares usernames

	int size;
	int soFar = 0;
	struct stat st;
	stat("database.csv", &st);
	size = st.st_size;
	char buffer[size + 1];
	int fdData;
	int offset;



	printf("opening database:\n");
	fdData = open("database.csv", O_RDONLY, 0644);//remove creat manually for now
	printf("size: %d\n", size);

	printf("reading database:\n");
	read(fdData, buffer, size);
	char *p = strchr(buffer, '\n');
	*p = 0;
	offset = (int)(p - buffer);
	printf("offset: %d\n", offset);
	printf("first buffer: %s\n", buffer);
	printf("sizeof(buffer) %lu\n", sizeof(buffer));
	soFar += (offset + 1);
	size -= (offset + 1);
	printf("soFar: %d\n", soFar);
	printf("size: %d\n", size);
	lseek(fdData, soFar, SEEK_SET);

	while(size){
		read(fdData, buffer, size);
		buffer[size] = 0;
		printf("buffer: %s\n", buffer);
		char *p = strchr(buffer, '\n');
		printf("%s\n", p);
		*p = 0;
		offset = (int)(p - buffer);
		soFar += (offset + 1);
		size -= (offset + 1);
		printf("post buffer: %s\n", buffer);




		char *name =  strchr(buffer, ',');
		*name = 0;
		printf("buffer (username only): %s\n", buffer);
		printf("from client: %s\n", &s[1]);
		if (strcmp(&s[1], buffer) == 0){
			printf("user exists!\n");
			return 1;
		}

		printf("soFar: %d\n", soFar);
		printf("size: %d\n", size);
		lseek(fdData, soFar, SEEK_SET);


	}
	printf("user does not exist!\n");
	return 0; //for now
}
