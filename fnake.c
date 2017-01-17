#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define Width 60
#define Height 30

#include "fnake.h"

int main(){
  int height = 30;
  int width = 60;
  int grid[width][height];
  int Xc = 0;
  int Yc = 0;
  int input;
  initGrid(width, height, grid);
  printGrid(width, height, grid);

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
      editGrid(Xc, Yc, grid);
      cycleGrid(width, height, grid);
      printGrid(width, height, grid);
    }
  }


}

void initGrid(int width, int height, int grid[][Height]){
  int x,y;

  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      grid[x][y] = 0;
    }
  }
}

void cycleGrid(int width, int height, int grid[][Height]){
  int x,y;
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      if (grid[x][y] != 0){
        grid[x][y]--;
      }
    }
  }
}

int checkCollisions(int Xc, int Yc, int grid[][Height]){
  int height = Height;
  int width = Width;
  if (grid[Xc][Yc] != 0){
    return 1;
  }else{
    return 0;
  }
}

void editGrid(int Xc, int Yc, int grid[][Height]){
  if (checkCollisions(Xc, Yc, grid)){
    printf("Collision Found");
    printf("%d\n", grid[Xc][Yc]);
    printf("%d, %d\n", Xc, Yc );
    grid[Xc][Yc] = 6;
  }else{
    grid[Xc][Yc] = 6;
  }
}

void printGrid(int width, int height, int grid[][Height]){
  int x,y;

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      if (grid[x][y] == 0){
        putchar('-');
      }else{
        putchar('S');
      }
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
