#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "verifications.h"
#include "solve.h"
#include "files.h"


/*
Function that helps the player by revealing a cell.
Returns 1 if all goes well, 0 if the grid is not resolvable.
*/
int help(int grid[9][9])
{
    int k, l, grid_copy[9][9], possible, finished;


    // solve a copy_grid of the grid
    copy_grid(grid, grid_copy);
    possible = solve(grid_copy);

    verif_finish(grid, &finished);
    if (finished) 
        return 0;
    if (!possible) 
        return 0;
    else
    {
        // We give a cell at random
        do
        {
            k = (int)((rand()/(double)RAND_MAX)*8);
            l = (int)((rand()/(double)RAND_MAX)*8);
        } while(grid[k][l] != 0);

        grid[k][l] = grid_copy[k][l]; // set the values of the original grid as the values of the solved copy_grid grid

        return 1;

    }
}
