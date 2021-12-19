#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <conio.h>  


void yellow();   
void reset();
void storeNeighbors(char *grid, int *neighborGrid, int rows, int columns);
bool gridIsDead(char *grid, int rows, int columns);
void makeAlive(char *grid, int i, int j, int columns);
void makeDead(char *grid, int i, int j, int columns);
void adjustGrid(char *grid, int rows, int columns);
void randomizeGrid(char *grid, int columns, int rows);
char printMenu();
void initializeGrid(char *grid, int rows, int columns);
void printGrid(char *grid, int rows, int columns);
void simulate(char *grid, int * neighborGrid, int rows, int columns);
int countNeighbors(char *grid, int loci, int locj, int rows, int columns);

int main(void)
{
    system("cls"); // or
    // system("clear");

    yellow();   printf("\n\n     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"); reset();
    printf("-----------------------------------------------------------");
    yellow();   printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");   reset();
    printf("\n     -------------------------------Welcome to Conway's Game of Life (a.k.a. The 0 Player Game)------------------------------\n");
    yellow();   printf("     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"); reset();
    printf("-----------------------------------------------------------");
    yellow();   printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n\n\n\n");   reset();
    printf("(Open on full screen for best experience.)\n\n\n\n");

    printf("Here are the rules for this game in short;\nFor each generation of cells in a grid:\n \n");
    printf("\t1. Any live cell with two or three live neighbours survives.\n");
    printf("\t2. Any dead cell with three live neighbours becomes a live cell.\n");
    printf("\t3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.\n\n\n");
    int rows, columns;  

    printf("Enter the number of rows and columns you want for the grid. (Preferrably 5 - 20 for rows and 5 - 40 columns)\n\n");
    printf("Rows: ");
    scanf("%d", &rows);
    printf("Columns: ");
    scanf("%d", &columns);

    printf("\n\n");

    // for the border around the grid
    rows += 2;
    columns += 2;

    char *grid = calloc(rows*columns, sizeof(char));
    if (grid == NULL) return 1;

    char option = printMenu();

    // initialize all cells to be dead
    initializeGrid(grid, rows, columns);

    if (option == '1')
    {
        randomizeGrid(grid, rows, columns);
    }
    else if (option == '2')
    {   
        // input the desiyellow coordinates
        system("cls");
        printGrid(grid, rows, columns);
        adjustGrid(grid, rows, columns);
    }
    
    system("cls");
    printf("\n This will be the initial grid: ");
    printGrid(grid, rows, columns);

    printf("Press ENTER to start the simulation\n ");
    getchar();
    getchar();

    int *neighborGrid = calloc(rows*columns, sizeof(int));
    if (neighborGrid == NULL)   return 1;
    
    while(!(gridIsDead(grid, rows, columns)))
    {
    simulate(grid, neighborGrid, rows, columns);
    system("cls");
    printGrid(grid, rows, columns);
    printf("Press Ctrl + C at any time to quit the simulation.\n");
    sleep(1);
    }

    printf("All the cells are dead! Game Over.\n");

    free(grid);
    free(neighborGrid);

    
    return 0;


}

void yellow()
{
    printf("\033[93m");
}

void reset()
{
    printf("\e[0m");
}

bool gridIsDead(char *grid, int rows, int columns)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            if (*(grid + i * columns + j) == '@')
                return false;
        }
    }

    return true;
}
void makeAlive(char *grid, int i, int j, int columns)
{
    *(grid + i * columns + j) = '@';
}

void makeDead(char *grid, int i, int j, int columns)
{
    *(grid + i * columns + j) = '.';
}

// mirror the edges to create the effect of wrapping around when reaching out of bound
void mirrorEdges(char *grid, int rows, int columns)
{
    /*
        Eg: 
    1st    . . . . . .
    2nd    . 1 2 3 4 .
    3rd    . 5 6 7 8 .
    4th    . a b c d .
    5th    . . . . . .

    make the 5th row be the same as the 2nd row ....so that when we look down from the bottom (4th) row, 
    we'll be looking back up to the upper (2nd) row. 
    and etc......

    */

    for (int j = 1; j < columns - 1; j++)
    {
        // fix the upper edges
        *(grid + 0 * columns + j) = *(grid + (rows - 2) * columns + j);
        // fix the lower edges
        *(grid + (rows - 1) * columns + j) = *(grid + 1 * columns + j);
    }

    for (int i = 1; i < rows - 1; i++)
    {
        // fix the left edges
        *(grid + i * columns + 0) = *(grid + i * columns + (columns - 2));

        // fix the right edges
        *(grid + i * columns + (columns - 1)) = *(grid + i * columns + 1);
    }
}

int countNeighbors(char *grid, int loci, int locj, int rows, int columns)
{
    mirrorEdges(grid, rows, columns);

    int count = 0;
    int location = loci * columns + locj;

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0)
                continue;
            if (*(grid + location + i * columns + j) == '@')
                count++;
        }
    }
    
    return count;
}

void storeNeighbors(char *grid, int *neighborGrid, int rows, int columns)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            *(neighborGrid + i * columns + j) = countNeighbors(grid, i, j, rows, columns);
        }
    }
}

/*
    1. Any live cell with two or three live neighbours survives.
    2. Any dead cell with three live neighbours becomes a live cell.
    3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.
*/
void simulate(char *grid, int * neighborGrid, int rows, int columns)
{
    storeNeighbors(grid, neighborGrid, rows, columns);

    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            char cell = *(grid + i * columns + j);
            int neighbours = *(neighborGrid + i * columns + j);

            //apply the rules 
            if (cell == '@')
            {
                if (neighbours < 2 || neighbours > 3)
                    makeDead(grid, i, j, columns);
            }
            else if (cell == '.')
            {
                if (neighbours == 3)
                    makeAlive(grid, i, j, columns);
            }
        }
    }
}

void randomizeGrid(char *grid, int rows, int columns)
{
    srand(time(NULL));
    printf("Enter a number from 0 - 100 signifying the probability of a cell to be alive: ");
    int prob;
    scanf("%d", &prob);
    int x;
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            x = rand() % 100;
            if (x < prob)
                makeAlive(grid, i, j, columns);
        }
    }
}

void printGrid(char *grid, int rows, int columns)
{
    printf("\n\n\n\n    ");

    // label the j coordinates on the top
    for (int i = 0; i < columns - 2; i++)
        printf("%2d  ", i + 1);
    printf("\n\n");

    // print without the borders
    for (int i = 1; i < rows - 1; i++)
    {
        // label the i coordinates on the left
        printf("%2d   ", i);
        for (int j = 1; j < columns - 1; j++)
        {
            if (*(grid + i * columns + j) == '@')
                yellow();
            printf("%c   ", *(grid + i *columns + j));
                reset();
        }
        printf("\n\n");
    }
}

void adjustGrid(char *grid, int rows, int columns)
{
    int count;
    printf("How many cells do you want to be alive initially? ");
    scanf("%d", &count);
    printf("\nEnter the i and j indexes of those cells (Example:- i: 2, j: 3 for the cell in the 2nd row and 3rd column): \n\n");
    int ic, jc;
    
    for (int i = 0; i < count; i++)
    {
        char c[3];
    
        switch (i)
        {
            case 0:
                strcpy(c, "st");
                break;
            case 1:
                strcpy(c, "nd");
                break;
            case 2:
                strcpy(c, "rd");
                break;
            default:
                strcpy(c, "th");
        } 
        printf("%d%s cell: \n\n", i + 1, c);
        printf("     i index: ");
        scanf("%d", &ic);
        printf("     j index: ");
        scanf("%d", &jc);
        printf("\n\n");

        makeAlive(grid, ic, jc, columns);
        system("cls");
        printGrid(grid, rows, columns);        
    }
}

char printMenu()
{
    printf("Press: \n");
    printf("       1. To randomize the initial grid.\n");
    printf("       2. To manually select the live cells in the initial grid.\n");
    printf("\n-----> ");
    char option;
    scanf(" %c", &option);
    printf("\n\n");
    return option;
}

void initializeGrid(char *grid, int rows, int columns)
{
    // also initialize the borders to be dead
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            makeDead(grid, i, j, columns);
        }
    }
}