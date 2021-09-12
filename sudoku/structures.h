// Constants for the difficulty of the levels (number of non-direct boxes to find)
#define EASY 25
#define MEDIUM 35
#define DIFFICULT 45


// Data structure to keep track of the possibilities of each cell
typedef struct nb
{
    int number[9];
}nb;

// Doubly-linked list structure for the solution (and therefore also the generation) of grids with guesswork
typedef struct grid_sol
{
    struct grid_sol *prev;
    int grid[9][9]; // We save the grid for each assumption to be able to go back if the assumption is not correct
    int n, i, j; // To know what assumption is made on this grid
    struct grid_sol *next;
}grid_sol;

