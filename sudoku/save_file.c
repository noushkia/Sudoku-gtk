#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "save_file.h"

/*
This function will save the grid in a txt file which can then be printed.
A grid is drawn to make everything more readable.
Boxes and regions are not delimited in the same way.
*/
void save_file(int grid[9][9], char* file_name)
{
    int i, j, n, k, l;
    FILE *file;
    file = fopen (file_name, "w+");

    l = 1;

    for(i = 0; i < 9; i++)
    {
        k = 0;
        for(j = 0; j < 9; j++)
        {
            n = grid[i][j];
            k++;
            if(n == 0) {fprintf(file, "   ");} // We replace the 0 by spaces to not print them
            else {fprintf(file, " %d ", n);}

            if(k == 3 || k == 6) {fprintf(file, " * ");}
            else if (k != 9) {fprintf(file, "|");}
        }
        if(l == 3 || l == 6) {fprintf(file, "\n* * * * * * * * * * * * * * * * * * *\n");}
        else if(l == 9) {fprintf(file, "\n");}
        else fprintf(file, "\n----------- * ----------- * ----------\n");
        l++;
    }
    fclose(file);
}
