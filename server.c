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
#include <signal.h>

#include "pipe_networking.h"
#include "server.h"

int main() {
	printf("[SERVER] booting...\n");
	signal(SIGINT, sigHandler);
	int sd, connection;
	char buffer [MESSAGE_BUFFER_SIZE];
	sd = server_setup();
 	while (1) {
		connection = server_connect(sd);
		printf("connection: %d\n", connection);
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

void sigHandler(int signum){
	if (signum == SIGINT){
		printf(" Shared memory will be deleted.\n"); 
		
		//read sm_fd from file
		int sm_fd, fd;
		fd = open("sm_filedesc.txt", O_RDWR, 0664);
		if (fd == -1){
			printf("error: %d - %s\n", errno, strerror(errno));
		}
		else {		
			read(fd, &sm_fd, sizeof(sm_fd));
			printf("sm_fd: %d\n", sm_fd);
			close(fd);
		}
		
		
		printf("sm_fd: %d\n", sm_fd);
		shmdt(&sm_fd); //let's say this detaches
		shmctl(sm_fd, IPC_RMID, 0);
		close(4);
    exit(0);
  }
}

void sub_server(int sd) {
	char buffer[MESSAGE_BUFFER_SIZE];
	int statusNumber = -1;
	int sm_fd;
	struct rooms * data;
	//write new sm_fd to file
	sm_fd = shmget(ftok("server.c", 174), 1048576, IPC_CREAT | 0664);
	int fd = open("sm_filedesc.txt", O_RDWR | O_CREAT, 0664);
	if (fd == -1)
		printf("error: %d - %s\n", errno, strerror(errno));
	else {
		write(fd, &sm_fd, sizeof(sm_fd));
		close(fd);
	}
	
	printf("sm_fd: %d\n", sm_fd);
	
	data = (struct rooms *) shmat(sm_fd, 0, 0);
  	
  	
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
			printf("data[x].roomName (out of function): %s\n", data[0].roomName);	
			printf("data[x].capacity (out of function): %d\n", data[0].capacity);	
			printf("data[x].player1 (out of function) %s\n", data[0].player1);
			strcpy(buffer, temp);
			
		}if (statusNumber == 6){ // refresh lobby
			char* temp = roomsToString(data);
			printf("(debug) roomsToString(data): %s\n", temp);
			strcpy(buffer, temp);
		}if (statusNumber == 7){ // refresh room
			char* temp = currentRoomToString(data, buffer);
			strcpy(buffer, temp);
		}if (statusNumber == 8){ // leave room
			char* temp = leaveRoom(buffer, data);
			printf("(debug) leaveRoom(data): %s\n", temp);
			//strcpy(buffer, temp);
		}if (statusNumber == 9){ // ready
			readyPlus(data, buffer);
			char* temp = "done";
			strcpy(buffer, temp);
		}if (statusNumber == 0){
			char * temp = startGame(data, buffer);
			strcpy(buffer, temp);
		}if ((int) buffer[0] == 105){ //(int)'i'
			writeMove(data, buffer);
		}if ((int) buffer[0] == 111){ //(int)'o'
			char* temp = readMove(data, buffer);
			strcpy(buffer, temp);
		}
		printf("(debug) sending to client: %s\n", buffer);
		write(sd, buffer, sizeof(buffer));
	}
}

void writeMove(struct rooms* data, char* buffer){
	//"i w roomname"
	printf("writeMove() received buffer: %s\n", buffer);
	char* out;
	int x;
	char buffer_roomName[100];
	
	strcpy(buffer_roomName, &strchr(buffer, ' ')[1]);
	printf("writeMove() buffer_roomName: %s\n", buffer_roomName);
	
	while (strcmp(data[x].roomName, "") != 0){
		printf("Join: looking for roomName: %s\n", buffer_roomName);
		printf("Comparing to data[x].roomName: %s\n", data[x].roomName);
		if (! strcmp(buffer_roomName, data[x].roomName)){
			printf("buffer[2]: %c", buffer[2]);
			data[x].input = buffer[2];
		}
		x++;
	}
}

char *readMove(struct rooms *data, char * buffer){
	char * out;
	int x = 0;
	char buffer_roomName[100];
	strcpy(buffer_roomName, &strchr(buffer, ' ')[1]);
	
	
	while (strcmp(data[x].roomName, "") != 0){
		printf("Join: looking for roomName: %s\n", buffer_roomName);
		printf("Comparing to data[x].roomName: %s\n", data[x].roomName);
		if (! strcmp(buffer_roomName, data[x].roomName)){
			buffer[2] = data[x].input;
			printf("buffer[2]: %c", buffer[2]);
		}
		x++;
	}
}




char * joinRoom(char * buffer, struct rooms * data){
	//buffer = "4!join roomName userName"
	char * out;
	int active = 0;
	int x = 0;
	char * temp;
	char userName[100];
	char roomName[100];

	//get relevant data from buffer sent by client
	temp = strrchr(buffer, ' ');
	strcpy(userName, &temp[1]);
	*temp = 0;
	temp = strchr(buffer, ' ');
	strcpy(roomName, &temp[1]);
	
	while (strcmp(data[x].roomName, "") != 0){
		printf("Join: looking for roomName: %s\n", roomName);
		printf("Comparing to data[x].roomName: %s\n", data[x].roomName);
		active = 0;
		if (! strcmp(roomName, data[x].roomName)){
			if (strcmp(data[x].player1, "") != 0) active++;
			if (strcmp(data[x].player2, "") != 0) active++;
			if (strcmp(data[x].player3, "") != 0) active++;
			if (strcmp(data[x].player4, "") != 0) active++;
			if ((active == data[x].ready || active == data[x].capacity) && active != 0){
				out = "full/in";
				return out;
			}
			//room found and can join
			printf("active after checking for empty slots: %d\n", active);
			if (active == 0){
				strcpy(data[x].player1, userName);
			}else if (active == 1){
				strcpy(data[x].player2, userName);
			}else if (active == 2){
				strcpy(data[x].player3, userName);
			}else if (active == 3){
				strcpy(data[x].player4, userName);
			}else{
				printf("Something went wrong.\n");
			}
			out = roomToString(data, x);
			return out;
		}
		x++;
	}
	out = "DNE";
	return out;
}

char * createRoom(char* buffer, struct rooms * data){
	//buffer = "5!create roomName userName"
	char* out;
	int x = 0;
	int y = 0;
	char* temp;
	char userName[100];
	char roomName[100];

	//get relevant data from buffer sent by client
	temp = strrchr(buffer, ' ');
	strcpy(userName, &temp[1]);
	printf("create: userName = %s\n", userName);
	*temp = 0;
	printf("buffer: %s\n", buffer);
	temp = strchr(buffer, ' ');
	strcpy(roomName, &temp[1]);
	printf("create: roomName = %s\n", roomName);
	
	if (doesExist(roomName, data)){ // check if room already exists
		out = strcpy(out, "exists");
		return out;
	}
	
	while (data[x].capacity){
		printf("data[x].capacity: %d\n", data[x].capacity);
		printf("data[x].roomName: %s\n", data[x].roomName);
		x++;
	}

	printf("WHAT IS x?: %d\n", x);
	strcpy(data[x].player1, userName);
	printf("data[x].player1 %s\n", data[x].player1);
	strcpy(data[x].roomName, roomName);
	printf("data[x].roomName: %s\n", data[x].roomName);
	data[x].capacity = MAX;
	printf("data[x].capacity: %d\n", data[x].capacity);
	data[x].ready = 0;
	printf("data[x].ready: %d\n", data[x].ready);
	
	strcpy(out, "success");
	return out;
}

int doesExist(char* roomName, struct rooms * data){
	printf ("Checking if room exists: %s\n", roomName);
	int x = 0;
	while (data[x].capacity){
		printf("Comparing (%s) to (%s)\n", data[x].roomName, roomName);
		if (! strcmp(data[x].roomName, roomName)){
			printf("exists\n");
			return 1;
		}
		x++;
	}
	printf("does not exist\n");
	return 0;
}

char* leaveRoom(char* buffer, struct rooms* data){
	//buffer = "8!leave roomName userName"
	char* out;
	char* temp;
	char buffer_roomname[100];
	char buffer_username[100];
	
	//get relevant data from buffer sent by client
	temp = strrchr(buffer, ' ');
	strcpy(buffer_username, &temp[1]);
	printf("(debug) leaveRoom(): username = %s\n", buffer_username);
	*temp = 0;
	printf("(debug) leaveRoom(): buffer =  %s\n", buffer);
	temp = strchr(buffer, ' ');
	strcpy(buffer_roomname, &temp[1]);
	printf("(debug) leaveRoom(): roomname = %s\n", buffer_roomname);
	
	//find location of the room you're leaving
	int pos = 0;
	while (strcmp(data[pos].roomName, "") != 0){
		if (strcmp(data[pos].roomName, buffer_roomname) == 0)
			break;
		pos++;
	}
	printf("(debug) leaveRoom(): pos of room = %d\n", pos);
	
	//modify ready num
	data[pos].ready--; 
	printf("(debug) leaveRoom(): read # = %d\n", data[pos].ready);
	
	//locate player to be removed and shift other players accordingly
	int active = 0;
	if (strcmp(data[pos].player1, "") != 0) active++;
	if (strcmp(data[pos].player2, "") != 0) active++;
	if (strcmp(data[pos].player3, "") != 0) active++;
	if (strcmp(data[pos].player4, "") != 0) active++;
	printf("(debug) leaveRoom(): number of players in room =  %d\n", active);
	if (strcmp(data[pos].player4, buffer_username) == 0){
		printf("(debug) removing player4...\n");
		strcpy(data[pos].player4, "");
	}
	else if(strcmp(data[pos].player3, buffer_username) == 0){
		printf("(debug) removing player3...\n");
		if (active == 4){
			strcpy(data[pos].player3, data[pos].player4);
			strcpy(data[pos].player4, "");
		}
		if (active == 3){
			strcpy(data[pos].player3, "");
		}
	}
	else if(strcmp(data[pos].player2, buffer_username) == 0){
		printf("(debug) removing player2...\n");
		if (active == 4){
			strcpy(data[pos].player2, data[pos].player3);
			strcpy(data[pos].player3, data[pos].player4);
			strcpy(data[pos].player4, "");
		}
		if (active == 3){
			strcpy(data[pos].player2, data[pos].player3);
			strcpy(data[pos].player3, "");
		}
		if (active == 2){
			strcpy(data[pos].player2, "");
		}
	}
	else if(strcmp(data[pos].player1, buffer_username) == 0){
		printf("(debug) removing player1...\n");
 		if (active == 4){
			strcpy(data[pos].player1, data[pos].player2);
			strcpy(data[pos].player2, data[pos].player3);
			strcpy(data[pos].player3, data[pos].player4);
			strcpy(data[pos].player4, "");
		}
		if (active == 3){
			strcpy(data[pos].player1, data[pos].player2);
			strcpy(data[pos].player2, data[pos].player3);
			strcpy(data[pos].player3, "");
		}
		if (active == 2){
			strcpy(data[pos].player1, data[pos].player2);
			strcpy(data[pos].player2, "");
		}
		if (active == 1){
			strcpy(data[pos].player1, "");
		}
	}
	else printf("(debug) leaveRoom(): error -- player wasn't in room to begin with?\n");
	
	//check name shifting
	printf("(debug) CHECKING TO SEE IF NAMES WERE SHIFTED PROPERLY\n");
	printf("(debug) leaveRoom(): data[pos].player1 = %s\n", data[pos].player1);	
	printf("(debug) leaveRoom(): data[pos].player2 = %s\n", data[pos].player2);
	printf("(debug) leaveRoom(): data[pos].player3 = %s\n", data[pos].player3);
	printf("(debug) leaveRoom(): data[pos].player4 = %s\n", data[pos].player4);
	
	//send-off
	strcpy(out, buffer_roomname);
	//printf("(debug) leaveRoom(): final string = %s\n", out);
	return out;
}


char * roomToString(struct rooms * data, int x){
	char *out = (char *)malloc(sizeof(char) * 1000);
	
	printf("(debug) roomToString: data[%d].roomName = %s\n", x, data[x].roomName);
	strcat(out, data[x].roomName);
	strcat(out, " ");
	
	int capacity = data[x].capacity;
	int ready = data[x].ready;
	char* capacityStr = (char*)malloc(sizeof(int) * 2);
	char* readyStr = (char*)malloc(sizeof(int) * 2);
	sprintf(capacityStr, "%d", data[x].capacity);
	sprintf(readyStr, "%d", data[x].ready);
	printf("(debug) roomToString: data[%d].capacity = %s\n", x, capacityStr);
	printf("(debug) roomToString: data[%d].ready = %s\n", x, readyStr);
	strcat(out, capacityStr);
	strcat(out, " ");
	strcat(out, readyStr);
	strcat(out, " ");
	
	if (strcmp(data[x].player1, "") != 0){
			strcat(out, data[x].player1);
			strcat(out, " ");
	}
	if (strcmp(data[x].player2, "") != 0){
			strcat(out, data[x].player2);
			strcat(out, " ");
	}
	if (strcmp(data[x].player3, "") != 0){
			strcat(out, data[x].player3);
			strcat(out, " ");
	}
	if (strcmp(data[x].player4, "") != 0){
			strcat(out, data[x].player4);
			strcat(out, " ");
	}
	
	*(strrchr(out,' '));
	printf("(debug) final out string for roomToString(): %s\n", out);
	return out;
}

char * roomsToString(struct rooms * data){
	char *out = (char *)malloc(sizeof(char) * 1000);
	int x = 0;
	
	//roomA 4 2 karol reo?roomB 4 0 god?roomC 4 3 brown platek dw k
	while (strcmp(data[x].roomName, "") != 0){
		printf("data[x].roomName = %s\n", data[x].roomName);
		strcat(out, data[x].roomName);
		strcat(out, " ");

		char* target = (char*)malloc(sizeof(int) * 2);
		char* target2 = (char*)malloc(sizeof(int) * 2);
		
		sprintf(target, "%d", data[x].capacity);
		sprintf(target2, "%d", data[x].ready);
		
		strcat(out, target);
		strcat(out, " ");
	  strcat(out, target2);
		strcat(out, " ");
		printf("rerex\n");
		printf("x: %d\n", x);
		
		if (strcmp(data[x].player1, "") != 0){
			strcat(out, data[x].player1);
			strcat(out, " ");
		}
		if (strcmp(data[x].player2, "") != 0){
			strcat(out, data[x].player2);
			strcat(out, " ");
		}
		if (strcmp(data[x].player3, "") != 0){
			strcat(out, data[x].player3);
			strcat(out, " ");
		}
		if (strcmp(data[x].player4, "") != 0){
			strcat(out, data[x].player4);
			strcat(out, " ");
		}
		printf("out after adding players: %s\n", out);
			
		printf("rerex\n");
		*(strrchr(out, ' ')) = '?';
		x++;
	}
	
	printf("out after rered: %s\n", out);
	
	
	if (strcmp(out, "") != 0)
		*(strrchr(out, '?')) = 0;
	else {
		strcpy(out, "DNE");
	}
	printf("final out for refresh: %s\n", out);
	return out;
}

char * currentRoomToString(struct rooms * data, char * buffer){
	//incoming buffer: "7 roomname"
	printf("currentRoomToString() incoming buffer: %s\n", buffer);
	char* out = (char*)malloc(sizeof(char*) * 1000);
	char roomName[100];
	int x = 0;
	char temp[100];
	strcpy(roomName, &strchr(buffer, ' ')[1]);
	printf("currentRoomToString() roomname: %s\n", roomName);
	
	
	
	while (strcmp(roomName, data[x].roomName) != 0){
		x++;
	}
	
	strcpy(out, roomToString(data, x));
	printf("currentRoomToString out: %s\n", out);
	strcat(out, "\0");
	return out;
}

void readyPlus(struct rooms * data, char * buffer){
	int x = 0;
	char * roomName = (char *)malloc(sizeof(char) * 1000);
	strcpy(roomName, &buffer[2]);
	printf("roomName: %s\n", roomName);
	
	while (strcmp(roomName, data[x].roomName) == 0){
		x++;
	}
	x--;
	data[x].ready++;
	printf("Incrementing data[%d].roomName: %s to: %d\n", x, data[x].roomName, data[x].ready);
}

char* startGame(struct rooms * data, char * buffer){
	//buffer = "0 roomName userName"
	printf("startGame(): incoming buffer = %s\n", buffer);
	int x = 0;
	char* temp;
	char buffer_roomname[100];
	char buffer_username[100];
	temp = strrchr(buffer, ' ');
	strcpy(buffer_username, &temp[1]);
	*temp = 0;
	temp = strchr(buffer, ' ');
	strcpy(buffer_roomname, &temp[1]);
	printf("startGame(): buffer_username = %s\n", buffer_username);
	printf("startGame(): buffer_roomname = %s\n", buffer_roomname);

	
	while (strcmp(buffer_roomname, data[x].roomName) != 0){
		x++;
	}

	int y;
	int active = 0;
	if (strcmp(data[x].player1, "") != 0) active++;
	if (strcmp(data[x].player2, "") != 0) active++;
	if (strcmp(data[x].player3, "") != 0) active++;
	if (strcmp(data[x].player4, "") != 0) active++;
	
	
	if (strcmp(data[x].player1, buffer_username) == 0) y = 0;
	else if (strcmp(data[x].player2, buffer_username) == 0) y = 1;
	else if (strcmp(data[x].player3, buffer_username) == 0) y = 2;
	else if (strcmp(data[x].player4, buffer_username) == 0) y = 3;


	if (data[x].ready == active){
		char* playerNum = (char*)malloc(sizeof(char) * 4);
		printf("playerNum: %s\n", playerNum);
		sprintf(playerNum, "%d", y);
		strcat(playerNum, "\0");
		return playerNum;
	}
	else{
		char* playerNum = (char*)malloc(sizeof(char) * 4);
		printf("playerNum: %s\n", playerNum);
		sprintf(playerNum, "%d", -1);
		strcat(playerNum, "\0");
		return playerNum;
	}
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
