#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define Width 60
#define Height 30

#include "fnake.h"
#include "pipe_networking.h"

int main(){
  int height = 30;
  int width = 60;
  int grid[width][height];
  int Xc = 20;
  int Yc = 20;
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
      placeFood(grid);
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
        printf("Doing -1.\n");
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

void placeFood(int grid[][Height]){
  grid[25][25] = 1000;
}

int checkCollisions(int Xc, int Yc, int grid[][Height], int snakeLength){
  int height = Height;
  int width = Width;
  if (grid[Xc][Yc] == 1000){
    printf("Found food!.\n");
    return snakeLength + 1;
  }
  if (grid[Xc][Yc] != 0){
    return -1;
  }else{
    return 0;
  }
}

int editGrid(int Xc, int Yc, int grid[][Height], int snakeLength){
  if (checkCollisions(Xc, Yc, grid, snakeLength) == -1){
    printf("Collision found, you lost! \n");
    printf("%d\n", grid[Xc][Yc]);
    printf("%d, %d\n", Xc, Yc );
    grid[Xc][Yc] = snakeLength;
    exit(0);
  }else if (checkCollisions(Xc, Yc, grid, snakeLength) != 0){
    snakeLength = checkCollisions(Xc, Yc, grid, snakeLength);
    grid[Xc][Yc] = snakeLength;
    return snakeLength;
  }else{
    grid[Xc][Yc] = snakeLength;
    return snakeLength;
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
