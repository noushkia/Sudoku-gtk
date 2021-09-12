#include <stdio.h>
#include <stdlib.h>
#include "verifications.h"
#include "generate.h"


// Checks if a number n can go in a coordinate cell (i; j)
// returns 1 if it is empty and can contain n, 0 otherwise
int verify_cell(int grid[9][9], int n, int i, int j)
{
    int i_case, j_case, k, l;

    // We place ourselves in the cell of the upper right corner of the 3x3 square which contains the cell (i; j)
    if (i < 3) {i_case = 0;}
    else if (i < 6 && i >= 3) {i_case = 3;}
    else {i_case = 6;}

    if (j < 3) {j_case = 0;}
    else if (j < 6 && j >= 3) {j_case = 3;}
    else {j_case = 6;}



    if (grid[i][j] != 0) // Check if there is already a number in the cell, in which case the following steps are not necessary, no move is possible in this cell.
    {
        return 0;
    }

    else
    {
    // We check if there is already a cell containing n in this square
        for (k = i_case; k < i_case + 3;  k++) // There are two cells in addition to the starting one in the square (in row and in column)
        {
            for (l = j_case; l < j_case + 3;  l++ )
            {
                if (grid[k][l] == n)
                {
                    return 0; // If there is a cell in the square that contains n, no need to go any further: we stop and return 0
                }
            }
        }
    // If no cell of the square contains n, we check the row and the column
        for (k = 0; k < 9; k++)
        {
            if (grid[k][j] == n) // we test the column
            {
                return 0; // If there is already an n in the column, we stop and return 0
            }

            else if (grid[i][k] == n) // we test the row
            {
                return 0; // If there is already an n in the row, we stop and return 0
            }
        }

    return 1; // If we get this far it means that there is no n in the square, the column and the row, so the cell can contain n
    }
}

// We check all the boxes of the grid using the function verify_cell
void verif_possibilites(int grid[9][9], nb grid_poss[9][9])
{
    int i, j, n;
    for (i = 0; i < 9; i++)
    {
        for (j = 0;  j < 9;j++)
        {
            for (n = 0;  n < 9; n++)
            {
                grid_poss[i][j].number[n] = verify_cell(grid, (n+1), i, j);
            }
        }
    }
}


void verif_finish(int grid[9][9], int *finished)
{
    int i, j;
    *finished = 1; // We assign the value 1 to finished then we check if there are any empty cells, in which case finished takes the value 0 (= false, or unfinished)
    for (i= 0;  i < 9; i++)
    {
        for (j = 0; j < 9;  j++)
        {
            if(grid[i][j] == 0)
            {
                *finished = 0;
                return;
            }
        }
    }
}



int verif_impossible(int grid[9][9]) // Check if the grid is solvable (no boxes where there are no possibilities)
{
    int i, j, n, impossible = 0, move_possible;
    nb grid_poss[9][9] ;

    verif_possibilites(grid, grid_poss);

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            if(grid[i][j] == 0)
            {
                move_possible = 0;
                for(n = 0; n < 9; n++)
                {
                    if(grid_poss[i][j].number[n] != 0) {move_possible = 1;} // See if at least one move is possible
                }
                if(move_possible == 0) {impossible = 1;} // If no move is possible the grid is unsolvable
            }
        }
    }
    return impossible;
}


// checks if the grid is valid
int verif_valid(int grid[9][9], int mistakes[9][9])
{
    int i, j, n, valid, p;

    valid = 1;

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            mistakes[i][j] = 0;
            n = grid[i][j]; // Save the number in the cell
            grid[i][j] = 0; // We empty this cell to know if n can be placed in it
            p = verify_cell(grid, n, i, j); // Check if the move is possible
            grid[i][j] = n; // On replace n
            if(!p && n != 0) { // If the move is not possible the grid is not valid
                mistakes[i][j] = n;
                valid = 0;
            } 
        }
    }

    return valid;
}
