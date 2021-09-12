#include <stdio.h>
#include <stdlib.h>
#include "verifications.h"
#include "solve.h"
#include "files.h"
#include "generate.h"


int solve_without_assumption (int grid[9][9])
{
    int i = 0, j = 0, n = 0, posi = 0, posj = 0, posn = 0, nb_poss = 0, posi2 = 0, posj2 = 0, posn2 = 0, nb_poss2 = 0, k;
    int finished = 0;
    nb grid_poss[9][9];

    // By default we set all the possibilities to 1 (= true), then we do the test and we set to 0 if it is impossible
    for (i=0; i < 9; i++)
    {
        for (j = 0;j < 9 ; j++)
        {
            for (n = 0;  n < 9; n++)
            {
                grid_poss[i][j].number[n] = 1;
            }
        }
    }


    // We call the verif_possibilites function to know which digits we can put in each box
    verif_possibilites(grid, grid_poss);

    k = 1;

    while(!finished && k)
    {
        k = 0; // k is used to know if we have found a cell or if we are blocked
        for(i = 0; i < 9; i++)
        {
            for(j = 0; j < 9; j++)
            {
                nb_poss = 0;
                for(n = 0; n < 9; n++) // We see if for a given cell a move is possible
                {
                    if(grid_poss[i][j].number[n])
                    {
                        nb_poss++;
                        posn = n;
                        posi = i;
                        posj = j;
                    }
                }
                if (nb_poss == 1)
                {
                    grid[posi][posj] = posn + 1;
                    verif_possibilites(grid, grid_poss);
                    k = 1;
                }
            }
        }

        // We check if an n can only go to one cell of the row / column / square
        for(n = 0; n < 9; n++)
        {
            for(i = 0; i < 9; i++)
            {
                nb_poss = 0;
                nb_poss2 = 0;
                for(j = 0; j < 9; j++)
                {
                    if(grid_poss[i][j].number[n]) // parse the row
                    {
                        nb_poss++;
                        posn = n;
                        posi = i;
                        posj = j;
                    }
                    if(grid_poss[j][i].number[n]) // parse the column
                    {
                        nb_poss2++;
                        posn2 = n;
                        posi2 = j;
                        posj2 = i;
                    }
                }
                if(nb_poss == 1) // If n can only go to one place on the row we place it
                {
                    grid[posi][posj] = posn + 1;
                    verif_possibilites(grid, grid_poss);
                    k = 1;
                }
                if(nb_poss2 == 1) // If n can only go to one place on the column we place it
                {
                    grid[posi2][posj2] = posn2 + 1;
                    verif_possibilites(grid, grid_poss);
                    k = 1;
                }
            }
        }

    verif_finish(grid, &finished);
    }

    if(finished) {return 1;} // returns 1 if the grid is solved, and 0 if it cannot be solved completely
    else {return 0;}
}

int solve_with_assumption(grid_sol *last_element, int impossible)
{
    int i, j, n, is_empty;
    grid_sol assumption;

    assumption.next = last_element; // We place ourselves between the last and second last element
    assumption.prev = assumption.next->prev;



    i = assumption.next->i; // We get the coordinates for when we have to change an assumption
    j = assumption.next->j;
    n = assumption.next->n;



    if(impossible) // If impossible it is necessary to change an assumption
    {
        copy_grid(assumption.next->prev->grid, assumption.next->grid); // reset the grid

        if(n < 9) // We test another n (if we can)
        {
            n++;
            is_empty = verify_cell(assumption.next->grid, n, i, j); // We are looking for an n that can be placed in the cell
            while(n < 9 && !is_empty)
            {
                n++;
                is_empty = verify_cell(assumption.next->grid, n, i, j);
            }
            if(is_empty) // If we found a possible hit we test it
            {
                assumption.next->grid[i][j] = n; // We change the n in the grid
                assumption.next->n = n; // We save the assumption to be able to come back to it
            }
            else
            {
                impossible = 1;
                assumption.next->n = 9; // No n is suitable, so we want to move on to the next part
                solve_with_assumption(assumption.next, impossible); // we call the function again to go back in the list (we therefore execute the following part)
            }
        }


        else // Or if we've tested them all we go back to change an older assumption recursively
        {
            if(assumption.prev != NULL) // We do not delete the first cell
            {
                free(assumption.next); // We go back, so the next cell is no longer necessary
                assumption.next = assumption.prev; // go back
                assumption.prev = assumption.prev->prev;
                assumption.next->next = NULL; // There is nothing after
                impossible = 1;
                solve_with_assumption(assumption.next, impossible); // we call the function again to change the assumption (we therefore execute the part above again
            }
            else // This means that we are in the first link, so it shouldn't be impossible, so there is an error
            {
                return 0;
            }
        }
    }


    else // If not we make a new guess and continue
    {
        assumption.next->next = malloc(sizeof(grid_sol)); // add a new element in the linked list
        assumption.next->next->prev = assumption.next; // connect in both directions
        assumption.prev = assumption.next; // move the assumption
        assumption.next = assumption.next->next;
        assumption.next->next = NULL;
        copy_grid(assumption.prev->grid, assumption.next->grid); //initialize the grid

        i=0;
        j=0;
        is_empty = 0; // is_empty = 1 if the cell is empty, 0 if not.

        if(assumption.next->grid[i][j] == 0) {is_empty = 1;}

        while(i <= 8 && !is_empty) // We are looking for the first empty cell of the grid
        {
            while(j < 8 && !is_empty)
            {
                j++;
                if(assumption.next->grid[i][j] == 0) {is_empty = 1;}
            }
            if(!is_empty) // If the cell is not suitable we test another row
            {
                j = 0;
                i++;
            }
        }
        // Once we exit the loop the coordinates i, j correspond to an empty cell

        n=1;
        is_empty = verify_cell(assumption.next->grid, n, i, j);

        while(!is_empty && n < 9) // We are looking for the first n which can go in the box (there is only one, otherwise it's impossible)
        {
            n++;
            is_empty = verify_cell(assumption.next->grid, n, i, j);
        }

        if(n <= 9) // save the assumption to be checked later on
        {
            assumption.next->grid[i][j] = n; // We place n in the grid
            assumption.next->i = i; // We save the assumption to be able to change it if necessary
            assumption.next->j = j;
            assumption.next->n = n;
        }
    }
    return 1; // no error occured so we can proceed
}

int solve(int grid[9][9])
{
    int finished, impossible;
    int prov = 1;
    grid_sol prem, aux;

    finished = solve_without_assumption(grid); // We try to solve without guesswork
    impossible = verif_impossible(grid);
    if(impossible)
        return 0; // the grid is unsolvable

    if(!finished)
    {
        prem.next = NULL; // Initialization of all prem variables
        prem.prev = NULL;
        prem.i = -1; // No cell modified
        prem.j = -1;
        prem.n = 0;
        copy_grid(grid, prem.grid); // copy the grid

        aux.next = &prem; // Aux (which allows us to browse the list) points to the first element (then it will be moved if necessary)

        do
        {
            impossible = verif_impossible(aux.next->grid);
            prov = solve_with_assumption(aux.next, impossible);
            aux.next = &prem; // We go back to the beginning of the list
            while(aux.next->next != NULL) {aux.next = aux.next->next;} // we place ourselves at the last link (it may have been changed)
            finished = solve_without_assumption(aux.next->grid); // After each assumption we see if this allowed us to solve other cells
        } while(!finished && prov); // We continue as long as the grid is not completely solved


        copy_grid(aux.next->grid, grid); // copy the grid

        // We free the whole chain
        aux.next = prem.next;
        while(aux.next->next != NULL)
        {
            aux.prev = aux.next;
            aux.next = aux.next->next;
            free(aux.prev);
        }
        free(aux.next); // We release the last element of the chain
    }
    return 1;
}
