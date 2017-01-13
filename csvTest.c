#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  char name[100];
  printf("Enter username: ");
  fgets(name, sizeof(name), stdin);
  
  authenticate();
  return 0;
}

void authenticate(char* data[]){
  if (strcmp(data[2], "register") == 0)
    return registration(data[0], data[1]);
  if (strcmp(data[2], "login") == 0)
    return logination(data[0], data[1]);
  else
    return "error!";	    
}

int userExists(char* user){
  return 0; 
}

void registration(char* user, char* pass){
}

void logination(char* user, char* pass){
}



  printf("Enter username: ");
  fgets(name, sizeof(name), stdin);
  *(strchr(input, '\n'));
}
