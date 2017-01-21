#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <termios.h>
#include <assert.h>
#include <string.h>

#define Width 50
#define Height 30

#include "fnake.h"
#include "pipe_networking.h"

int main(){

  //printf ("\e[9;1t\n");
  //printf ("\e[9;1t\n");
  //printf ("\e[9;1t\n");
  //printf ("\e[8;50;100t");
  //terminal settings---------------------------
  int c = 0;
  struct termios org_opts, new_opts;
  int res = 0;
    //-----  stores old settings -----------
  res=tcgetattr(STDIN_FILENO, &org_opts);
  assert(res==0);
      //---- sets new terminal parms --------
  printf("Press a key.\n");
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c = getchar();



  int height = Height;
  int width = Width;
  int grid[width][height];
  int Xc = 3;
  int Yc = 3;
  int snakeLength = 6;
  int input;
  initGrid(width, height, grid);
  printGrid(width, height, grid);
  placeFood(grid);

  while(1){
    input = getInput();
    if (input != '\n'){
      printf("input: %c\n", input);
      printf("-----------------------------------\n");

      if (input == 'w'){
        Yc -= 1;
      }
      if (input == 's'){
        Yc += 1;
      }
      if (input == 'a'){
        Xc -= 1;
      }
      if (input == 'd'){
        Xc += 1;
      }
      snakeLength = editGrid(Xc, Yc, grid, snakeLength);
      cycleGrid(width, height, grid);
      printGrid(width, height, grid);
    }
  }
}

void initGrid(int width, int height, int grid[][Height]){
  int x,y;

  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      if (x == 0 || y == 0 || x == width - 1 || y == height - 1){
        //printf("Doing -1.\n");
        grid[x][y] = -1;
      }else{
        grid[x][y] = 0;
      }
    }
  }
}

void cycleGrid(int width, int height, int grid[][Height]){
  int x,y;
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      if (grid[x][y] != 0 && grid[x][y] != -1 && grid[x][y] != 1000){
        grid[x][y]--;
      }
    }
  }
}

void placeFood(int grid[][Height]){ //randomly places food
  int randX = 0;
  int randY = 0;
  while (grid[randX][randY] != 0){
    randX = (rand()%(Width - 2)) + 1;
    randY = (rand()%(Height - 2)) + 1;
  }
  grid[randX][randY] = 1000;
}

int checkCollisions(int Xc, int Yc, int grid[][Height], int snakeLength){
  int height = Height;
  int width = Width;
  if (grid[Xc][Yc] == 1000){
    printf("Found food!.\n");
    placeFood(grid);
    return snakeLength + 1;
  }
  if (grid[Xc][Yc] != 0){
    return -1;
  }else{
    return 0;
  }
}

int editGrid(int Xc, int Yc, int grid[][Height], int snakeLength){
  int status = checkCollisions(Xc, Yc, grid, snakeLength);
  int x, y;
  if (status == -1){ //lost
    printf("Collision found, you lost! \n");
    printf("%d\n", grid[Xc][Yc]);
    printf("%d, %d\n", Xc, Yc );
    grid[Xc][Yc] = snakeLength;
    exit(0);
  }else if (status != 0){ //found food
    snakeLength = status;
    grid[Xc][Yc] = snakeLength;
    incrementSnakeLength(grid, snakeLength);
    return snakeLength;
  }else{ //normal movement
    grid[Xc][Yc] = snakeLength;
    return snakeLength;
  }
}

void incrementSnakeLength(int grid[][Height], int snakeLength){
  int x, y;
  for (y = 0; y < Height; y++){
    for (x = 0; x < Width; x++){
      if (grid[x][y] != 0 && grid[x][y] != -1 && grid[x][y] != 1000 && grid[x][y] != snakeLength){
        grid[x][y]++;
      }
    }
  }
}

void printGrid(int width, int height, int grid[][Height]){
  int x,y;

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      if (grid[x][y] == 0){
        putchar('-');
      }else if (grid [x][y] == -1){
        printf(ANSI_COLOR_RED);
        putchar('#');
        printf(ANSI_COLOR_RESET);
      }else if (grid [x][y] == 1000){
        printf(ANSI_COLOR_CYAN);
        putchar('F');
        printf(ANSI_COLOR_RESET);
      }else{
        printf(ANSI_COLOR_YELLOW);
        putchar('S');
        printf(ANSI_COLOR_RESET);
      }
    }
    putchar('\n');
  }
}

void printGridTrue(int width, int height, int grid[][Height]){
  int x,y;

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      putchar(grid[x][y] + '0');
    }
    putchar('\n');
  }
}

char getInput(){
  char out;
	printf("Type:");
	scanf("%c", &out);
  printf("%c\n", out);
  return out;
}





//cleaner solution for getting single characters from the terminal.
/*

int getch(void) {
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return(c);
*/
