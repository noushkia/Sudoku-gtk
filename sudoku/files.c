#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "files.h"


int read_file(int grid[9][9], char* file_name) // Returns 1 if it works, 0 otherwise
{
    int i, j, n, k, nb = 0, time_passed = 0;
    FILE *file = NULL;
    char *c;
    char time[20];
    // We open the file
    file = fopen(file_name, "r");


    if(file == NULL) { // if the file failed to be opened
        return 0;
    }

    else
    {
        for(i = 0; i < 9; i++)
        {
            for(j = 0; j < 9; j++)
            {
                grid[i][j] = fgetc(file) - 48; // switch from ASCII to decimal
            }
        }
        fgetc(file);
        // Now get the time passed last time the grid was saved
        c = (char *) malloc(1);
        memset(c, 0, sizeof(c));
        memset(time, 0, sizeof(time));
        while((c[0] = fgetc(file)) != '\n')
        {
            strcat(time, c);
            memset(c, 0, sizeof(c)); // reset c so that the read charachter doesen't get repeated
        }

        fclose(file);

        if (time != NULL) // If the time was saved
            sscanf(time, "%d", &time_passed);

        return time_passed; // return time spent in the game last time it was saved

    }
}

int save_grid(int grid[9][9], char* file_name, int time_passed) // Returns 1 if it works, 0 otherwise
{
    int i, j;
    FILE *file = NULL;

        file = fopen(file_name, "w"); // We open the file


    if(file == NULL) // if the file failed to be opened
    {
        return 0;
    }
    else
    { // print the grid on the file
        for(i = 0; i < 9; i++)
        {
            for(j = 0; j < 9; j++)
            {
                fprintf(file, "%d", grid[i][j]);
            }
        }
        fprintf(file, "\n");
        fprintf(file, "%d", time_passed);
        fprintf(file, "\n");

        fclose(file);
        return 1;
    }
    return 0; //Something went wrong
}

void copy_grid(int grid[9][9], int grid_copy[9][9]) // copy the grid to grid_copy
{
    int i, j;
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            grid_copy[i][j] = grid[i][j];
        }
    }
}

int compare_grids(int grid1[9][9], int grid2[9][9]) // Returns 1 they are equal, 0 otherwise
{
    int i, j;
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            if (grid1[i][j]!=grid2[i][j]) return 0;
        }
    }

    return 1;
}
