#include "structures.h"

// Checks if a number n can go in a coordinate cell (i; j).
int verify_cell(int grid[9][9], int n, int i, int j);

// Checks all the possibilities of all the cells in the game.
void verif_possibilites(int grid[9][9], nb grid_poss[9][9]);

// Checks if the game is finished, therefore if all the cells are filled.
void verif_finish(int grid[9][9], int *finished);

//  checks if a grid is solvable
int verif_impossible(int grid[9][9]);

// checks if a grid is valid
int verif_valid(int grid[9][9], int mistakes[9][9]);
