#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "pipe_networking.h"

void process(char *);
int userExists(char *);
char * authenticate(char *);
int checkPassword(char *);
void sub_server(int);
void addAccount(char *);

int main() {

	int sd, connection;


	char buffer [MESSAGE_BUFFER_SIZE];

	sd = server_setup();
 	while (1) {
		connection = server_connect( sd );
		int f = fork();
	  if ( f == 0 ) {
			close(sd);
			sub_server( connection );
			exit(0);
	 	}
	 	else {
		 	close( connection );
	 	}
 	}
 	return 0;
}

void sub_server( int sd ) {

	int statusNumber = -1;
  char buffer[MESSAGE_BUFFER_SIZE];
	while(1){
		read( sd, buffer, sizeof(buffer) );
		printf("Getting statusNumber\n");
		statusNumber = atoi(&buffer[0]);
		printf("statusNumber: %d\n", statusNumber);
		printf("buffer: %s\n", buffer);
		//HOW I SEE THIS: Client not only sends what user inputs but an extra bit, a number that indicates what the server must do with the input
		if (statusNumber == 1){ //check if username exists
			strcpy(buffer, authenticate(buffer));
		}
		if (statusNumber == 2){ //if u exists, check if password is correct
			int out = checkPassword(buffer);
			if (out){
				printf("SERVER-S\n");
				strcpy(buffer, "Login Successful");
			}else{
				printf("SERVER-F\n");
				strcpy(buffer, "Login Failed"); 
			}

		}
		if (statusNumber == 3){
			addAccount(buffer);
			strcpy(buffer, "Login Successful");
			//while(1) printf("register OK hahaha!\n");
		}

		printf("buffer (sending back to client): %s\n", buffer);
		write( sd, buffer, sizeof(buffer));
	}
}


char * authenticate(char * s) { //handles all authentication cases
	printf("authenticating\n");
	if (userExists(s)){
		//prompt for password
		return "User already exists in database, enter your password.";
	}
	else{
		//ask for password, password confirmation, and entry (username and password) if registration successful
		return s;//"New user, please enter password.";
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



	printf("opening database...\n");
	fdData = open("database.csv", O_RDONLY, 0644);//remove creat manually for now
	printf("size: %d\n", size);

	printf("reading database...\n");
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

void addAccount(char *s){
	char* password = strsep(&s, "?");
	char* username = s;
	printf("username: %s\n", username);
	printf("password: %s\n", password);
	strcat(username, ",");
	strcat(username, &password[1]);
	strcat(username, "\n");
	char* entry = username;
	printf("entry: %s\n", entry);
	printf("strlen(entry): %lu\n", strlen(entry));

	printf("opening database...\n");
	int fd = open("database.csv", O_RDWR | O_APPEND, 0777);
	if (fd == -1) 
		printf("error: %d - %s\n", errno, strerror(errno));
	printf("adding entry to database...\n");
	write(fd, entry, strlen(entry));
	close(fd);
}
