#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "verifications.h"
#include "solve.h"
#include "files.h"
#include "generate.h"

// function to evaluate the difficulty of the current grid
int evaluate_level (int grid[9][9], int nb_cells)
{
    int lvl, i, j, n, nb_directs = 0, nb_to_find = 0, nbre;
    nb grid_poss[9][9];
    verif_possibilites(grid, grid_poss);

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            nbre = 0;
            for(n = 0; n < 9; n++)
            {
                if(grid_poss[i][j].number[n] == 1) {nbre++;}
            }
            if(nbre == 1) {nb_directs++;} // We count the number of boxes where there is only one possible number to be placed
        }
    }
    nb_to_find = nb_cells - nb_directs; // In the number of boxes to be found we remove direct boxes

    // The level is assigned according to the number of non-direct boxes to be solved
    if(nb_to_find >= EASY && nb_to_find < MEDIUM) {lvl = 1;} // easy
    else if(nb_to_find >= MEDIUM && nb_to_find < DIFFICULT) {lvl = 2;} // medium
    else if(nb_to_find >= DIFFICULT) {lvl = 3;} // hardest
    else {lvl = 0;} // If there are not enough boxes missing set the level as easiest

    return lvl;

}



void generate (int grid[9][9], int lvl)
{
    int i, j, n, nb, p, proceed = 1, lvl_actual, grid_test[9][9];

    srand(time(NULL)); // Initializing rand

// Initialization of the grid (we don't know what it contains, it could be an old grid)
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            grid[i][j] = 0;
        }
    }


// The 3 regions of the diagonal can be filled in randomly without worrying about rows or columns
    // Fill the region at the top left
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            n = (int)((rand()/(double)RAND_MAX)*9 +1); //+1 Because with the cast we risk obtaining 0
            if(n > 9) {n = 9;} // We check that we do not exceed the limits
            p = verify_cell(grid, n, i, j);
            while(!p)
            {
                if(n==9) {n=1;}
                else {n++;}
                p = verify_cell(grid, n, i, j);
            }
            grid[i][j] = n; // Once we get here, the generated number is valid
        }
    }

    for(i = 3; i < 6; i++) // Fill the region in the middle
    {
        for(j = 3; j < 6; j++)
        {
            n = (int)((rand()/(double)RAND_MAX)*9 +1); //+1 Because with the cast we risk obtaining 0
            if(n > 9) {n = 9;}
            p = verify_cell(grid, n, i, j);
            while(!p)
            {
                if(n==9) {n=1;}
                else {n++;}
                p = verify_cell(grid, n, i, j);
            }
            grid[i][j] = n;
        }
    }

    // Fill the region at the bottom right
    for(i = 6; i < 9; i++)
    {
        for(j = 6; j < 9; j++)
        {
            n = (int)((rand()/(double)RAND_MAX)*9 +1); //+1 Because with the cast we risk obtaining 0
            if(n > 9) {n = 9;}
            p = verify_cell(grid, n, i, j);
            while(!p)
            {
                if(n==9) {n=1;}
                else {n++;}
                p = verify_cell(grid, n, i, j);
            }
            grid[i][j] = n;
        }
    }


// The diagonal is generated, we can finish solving the grid by calling the solve function
    solve(grid);


// We obtain a complete grid, it is necessary to remove certain cells to make it playable
    proceed = 1;
    nb = 0;

    while(proceed)
    {
        i = (int)((rand()/(double)RAND_MAX)*8);
        j = (int)((rand()/(double)RAND_MAX)*8);
        if(grid[i][j] != 0)
        {
            n = grid[i][j];
            grid_test[i][j] = 0;
            p = solve_without_assumption(grid_test); // We check that there is only one possible solution
            if(p)
            {
                grid[i][j] = 0;
                nb++;
                lvl_actual = evaluate_level(grid, nb);
                if(lvl_actual == lvl) {proceed = 0;}
            }
            copy_grid(grid, grid_test);
        }
    }
}
