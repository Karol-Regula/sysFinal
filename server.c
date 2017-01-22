#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "pipe_networking.h"

#define MAX 7


struct rooms{
	char * roomName;
	int size;
	int ready;
	char ** userNames;
} rooms;


void process(char *);
int userExists(char *);
char * authenticate(char *);
int checkPassword(char *);
void sub_server(int);
void addAccount(char *);
char * joinRoom(char *, struct rooms *);
char * createRoom(char *, struct rooms *);
char * roomToString(struct rooms *, int);
char * roomsToString(struct rooms *);

int main() {
	printf("[SERVER] booting...\n");
	int sd, connection;
	char buffer [MESSAGE_BUFFER_SIZE];
	sd = server_setup();
 	while (1) {
		connection = server_connect(sd);
		int f = fork();
	  	if (f == 0) {
			close(sd);
			sub_server(connection);
			exit(0);
	 	}
	 	else close(connection);
 	}
 	return 0;
}

void sub_server(int sd) {
	int statusNumber = -1;
  	char buffer[MESSAGE_BUFFER_SIZE];
	
	struct rooms * data;
	int sdd;
	sdd = shmget(ftok("server.c", 174), 1048576, IPC_CREAT | 0664);
	data = (struct rooms *) shmat(sdd, 0, 0);
  	
  	
	while(1){
		read(sd, buffer, sizeof(buffer));
		statusNumber = atoi(&buffer[0]);
		printf("(debug) status number: %d\n", statusNumber);
		printf("(debug) received buffer: %s\n", buffer);
		if (statusNumber == 1){ //check if username exists
			strcpy(buffer, authenticate(buffer));
		}
		printf("(debug) status number: %d\n", statusNumber);
		if (statusNumber == 2){ //if u exists, check if password is correct
			int out = checkPassword(buffer);
			if (out){
				printf("[SERVER] login successful!\n");
				strcpy(buffer, "Login successful");
			}
			else{
				printf("[SERVER] login failed!\n");
				strcpy(buffer, "Login failed");
			}
		}
		if (statusNumber == 3){
			addAccount(buffer);
		  	printf("[SERVER] login successful!\n");
			strcpy(buffer, "Login successful");
		}
		if (statusNumber == 4){ // join room
			printf("statusnumber = 4 activated\n");
			char* temp = joinRoom(buffer, data);
			printf("line 92 temp is: \n");
			strcpy(buffer, temp);
			printf("status number 4 buffer: %s\n", buffer);
		}if (statusNumber == 5){ // create room
			char * temp = createRoom(buffer, data);
			printf("Line 97 temp: %s", temp);
			strcpy(buffer, temp);
			
		}if (statusNumber == 6){ // refresh room
			char* temp = roomsToString(data);
			strcpy(buffer, temp);
		}
		printf("(debug) sending to client: %s\n", buffer);
		write(sd, buffer, sizeof(buffer));
	}
}

char * joinRoom(char * buffer, struct rooms * data){
	//buffer = "4!join roomName userName"
	char * out;
	
	int x = 0;
	int y = 0;
	char * temp;
	char userName[100];
	char roomName[100];
	printf("here1\n");
	//get relevant data from buffer sent by client
	printf("here1\n");
	temp = strtok(buffer, " ");
	printf("here1\n");
	strcpy(userName, temp);
	printf("here1\n");
	char *p = strrchr(buffer, ' ');
	printf("here1\n");
	//*p = 0;
	printf("here2\n");
	temp = strtok(buffer, " ");
	printf("here2\n");
	strcpy(roomName, temp);
	printf("here2\n");
	while (data[x].size){
		printf("here2\n");
		if (! strcmp(roomName, data[x].roomName) && data[x].size && data[x].size != data[x].ready){
			//room found and can join
			y = 0;
			printf("here2\n");
			while(data[x].userNames[y]){
				y++;
			}
			if (y == MAX - 1 || (y == data[x].ready)){
				out = "full/in";
				printf("here3\n");
				return out;
			}else{
				printf("here2\n");
				strcpy(data[x].userNames[y], userName);
				//data[x].userNames[y] = userName;
				data[x].size++;
				out = roomToString(data, x); //returns all room data in the form of a string
				printf("here4\n");
				return out;
			}
		}
		x++;
	}
	printf("here5\n");
	out = "DNE";
	return out;
}

char * createRoom(char* buffer, struct rooms * data){
	//buffer = "5!create roomName userName"
	char* out;
	printf("cere1\n");
	int x = 0;
	int y = 0;
	char* temp;
	char userName[100];
	char roomName[100];
	printf("cere1\n");

	//get relevant data from buffer sent by client
	temp = strtok(buffer, " ");
	printf("cere1\n");
	strcpy(userName, temp);
	printf("cere1\n");
	char *p = strrchr(buffer, ' ');
	printf("cere1\n");
		*p = 0;
	printf("cere1\n");
	
	temp = strtok(buffer, " ");
	strcpy(roomName, temp);
	printf("cere1\n");
	
	while (data[x].size){
		x++;
	}
	printf("cere1\n");
	data[x].userNames[0] = userName;
	data[x].roomName = roomName;
	data[x].size = 1;
	data[x].ready = 0;
	printf("cere1\n");
	
	out = "success";
	printf("cere1\n");
	return out;
}

char * roomToString(struct rooms * data, int x){
	char * out;
	int len = 0;
	
	int size = data[x].size;
	int ready = data[x].ready;
	char userName[100];
	strcpy(userName, data[x].userNames[0]);
	char roomName[100];
	strcpy(roomName, data[x].roomName);
	
	out[len] = *roomName;
	len += strlen(roomName);
	out[len] = ' ';
	len++;
	out[len + 1] = size;
	out[len + 2] = ' ';
	
	
	return out;
}

char * roomsToString(struct rooms * data){
	char * out;
	int len = 0;
	int x = 0;
	while (data[x].roomName){
		strcpy(out, "hahah");
		//out[len] = *data[x].roomName;
		len += strlen(data[x].roomName);
		strcpy(&out[len + 1], " ");
		len++;
		x++;
	}
	return out;
	
}


char * authenticate(char * s) { //handles all authentication cases
	//printf("[SERVER] authenticating username...\n");
	if (userExists(s)){
		//prompt for password
		return "user exists";
	}
	else{
		//ask for password, password confirmation, and entry (username and password) if registration successful
		return s;//"New user, please enter password.";
	}
}

int checkPassword(char * s){
	printf("[SERVER] checking if password is correct...\n");
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

	fdData = open("database.csv", O_RDONLY, 0644);//remove creat manually for now
	printf("(debug) size: %d\n", size);

	printf("[SERVER] reading database...\n");
	read(fdData, buffer, size);
	char *p = strchr(buffer, '\n');
	*p = 0;
	offset = (int)(p - buffer);
	printf("(debug) offset: %d\n", offset);
	printf("(debug) first buffer: %s\n", buffer);
	printf("(debug) sizeof(buffer) %lu\n", sizeof(buffer));
	soFar += (offset + 1);
	size -= (offset + 1);
	printf("(debug) soFar: %d\n", soFar);
	printf("(debug) size: %d\n", size);
	lseek(fdData, soFar, SEEK_SET);

	while(size){
		read(fdData, buffer, size);
		buffer[size] = 0;
		printf("(debug) buffer: %s\n", buffer);
		char *p = strchr(buffer, '\n');
		printf("(debug) remaining database: %s\n", p);
		*p = 0;
		offset = (int)(p - buffer);
		soFar += (offset + 1);
		size -= (offset + 1);
		printf("(debug) post buffer: %s\n", buffer);
		char *r =  strchr(buffer, ',');
		temp = (int)((r - buffer) + 1);
		printf("(debug) temp: %d\n", temp);
		printf("(debug) buffer (password only): %s\n", &buffer[temp]);
		printf("(debug) from client: %s\n", &s[1]);
		if (strcmp(&s[1], &buffer[temp]) == 0){
			printf("[SERVER] saved password matches input -- success!\n");
			return 1;
		}

		printf("(debug) soFar: %d\n", soFar);
		printf("(debug) size: %d\n", size);
		lseek(fdData, soFar, SEEK_SET);
	}
	printf("[SERVER] saved password does not match input -- failure!\n");
	return 0;
}

int userExists(char * s){
	printf("[SERVER] checking if user already exists...\n");
	//reads line by line and compares usernames

	int size;
	int soFar = 0;
	struct stat st;
	stat("database.csv", &st);
	size = st.st_size;
	char buffer[size + 1];
	int fdData;
	int offset;

	fdData = open("database.csv", O_RDONLY, 0644);//remove creat manually for now
	printf("(debug) size: %d\n", size);
	printf("[SERVER] reading database...\n");
	read(fdData, buffer, size);
	char *p = strchr(buffer, '\n');
	*p = 0;
	offset = (int)(p - buffer);
	printf("(debug) offset: %d\n", offset);
	printf("(debug)first buffer: %s\n", buffer);
	printf("(debug) sizeof(buffer) %lu\n", sizeof(buffer));
	soFar += (offset + 1);
	size -= (offset + 1);
	printf("(debug) soFar: %d\n", soFar);
	printf("(debug) size: %d\n", size);
	lseek(fdData, soFar, SEEK_SET);

	while(size){
		read(fdData, buffer, size);
		buffer[size] = 0;
		printf("(debug) buffer: %s\n", buffer);
		char *p = strchr(buffer, '\n');
		*p = 0;
		offset = (int)(p - buffer);
		soFar += (offset + 1);
		size -= (offset + 1);
		printf("(debug) post buffer: %s\n", buffer);
		char *name =  strchr(buffer, ',');
		*name = 0;
		printf("(debug) buffer/username: %s\n", buffer);
		printf("(debug) from client: %s\n", &s[1]);
		if (strcmp(&s[1], buffer) == 0){
			printf("[SERVER] username located -- exists!\n");
			return 1;
		}
		printf("(debug) soFar: %d\n", soFar);
		printf("(debug) size: %d\n", size);
		lseek(fdData, soFar, SEEK_SET);
	}
	printf("[SERVER] username not located -- does not exist!\n");
	return 0;
}

void addAccount(char *s){
	char* password = strsep(&s, "?");
	char* username = s;
	printf("[SERVER] (debug) username: %s\n", username);
	printf("[SERVER] (debug) password: %s\n", password);
	strcat(username, ",");
	strcat(username, &password[1]);
	strcat(username, "\n");
	char* entry = username;
	printf("(debug) entry: %s\n", entry);
	printf("(debug) strlen(entry): %lu\n", strlen(entry));

	printf("[SERVER] Opening database...\n");
	int fd = open("database.csv", O_RDWR | O_APPEND, 0777);
	if (fd == -1)
		printf("error: %d - %s\n", errno, strerror(errno));
	printf("[SERVER] Adding entry to database...\n");
	write(fd, entry, strlen(entry));
	close(fd);
}
