#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define Width 60
#define Height 30

#include "fnake.h"

int main(){
  int height = 30;
  int width = 60;
  char grid[height][width];
  int Xc = 0;
  int Yc = 0;
  int input;
  initGrid(height, width, grid);
  printGrid(height, width, grid);

  while(1){
    input = getInput();
    //printf("input: %c\n", input);

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
    //cycleGrid(height, width, grid);
    printGrid(height, width, grid);
  }


}

void initGrid(int height, int width, char grid[][Width]){
  int x,y;

  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      grid[y][x] = ' ';
    }
  }
}

void cycleGrid(int height, int width, char grid[][Width]){
  int x,y;
  int temp;
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      printf("%c\n", grid[x][y]);
      //temp = atoi(grid[x][y]);
      if (temp != 0){
        //grid[x][y] = (temp - 1) + '0';
      }
    }
  }
}

void editGrid(int Xc, int Yc, char grid[][Width]){
  grid[Yc][Xc] = '5';
}

void printGrid(int height, int width, char grid[][Width]){
  int x,y;

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      putchar(grid[y][x]);
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
