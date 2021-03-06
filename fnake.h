#ifndef FNAKE_H
#define FNAKE_H

#define Width 25
#define Height 12

void printGrid(int, int, int [][Height]);
void printGridTrue(int, int, int [][Height]);
void initGrid(int, int, int [][Height]);
void cycleGrid(int, int, int [][Height]);
char getInput();
void placeFood(int [][Height]);
int editGrid(int, int, int [][Height], int);
int checkCollisions(int, int, int [][Height], int);
void incrementSnakeLength(int [][Height], int);

#endif