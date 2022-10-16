/*
    
    This is a program which simulates the propogation of fires
    through a forest. Integers are used to denote the state
    of each node on the 2D grid. The states are as follows:
    0 - Empty, 1 - Tree, 2 - Fire.
 
    This program uses neillsimplescreen.c and neillsimplescreen.h
    and must be compiled with the appropriate make file.
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "neillsimplescreen.h"

#define ROWS 30
#define COLS 80
#define GENS 1000
#define G    250
#define L    (G * 10)
#define WAIT 0.1

enum bool {false, true};
typedef enum bool bool;

void test();
void initGrid(int grid[ROWS][COLS]);
void printGrid(const int grid[ROWS][COLS]);
bool probGrowth(void);
bool probBurn(void);
void nextGen(const int old_grid[ROWS][COLS], int new_grid[ROWS][COLS]);
int  changeState(const int grid[ROWS][COLS], const int i, const int j);
bool isEmpty(const int state);
bool isTree(const int state);
bool isFire(const int state);
bool safe(const int y, const int x);
void copyGrid(const int src_grid[ROWS][COLS], int dst_grid[ROWS][COLS]);
void animateGrid(const int grid[ROWS][COLS], const double delay);

int main(void)
{
    int prev_gen[ROWS][COLS];
    int curr_gen[ROWS][COLS];
    int n;
    
    srand(time(NULL));
    test();
    
    initGrid(prev_gen);
    initGrid(curr_gen);
    
    neillclrscrn();
    
    for(n = 0; n < GENS; n++){
        nextGen(prev_gen, curr_gen);
        animateGrid(curr_gen, WAIT);
        copyGrid(curr_gen, prev_gen);
    }
    return 0;
}

void test()
{
    bool copy_flag;
    int i, x, y, growthCounter = 0, burnCounter = 0;
    int t1[ROWS][COLS];
    int t2[ROWS][COLS];
    int t3[ROWS][COLS];
    int t4[ROWS][COLS];
    int t4_cp[ROWS][COLS];
    
    /* Check if 2D grid is initialised with zeroes */
    initGrid(t1);
    
    assert(t1[0][0] == 0);
    assert(t1[ROWS-1][0] == 0);
    assert(t1[ROWS-1][COLS-1] == 0);
    assert(t1[0][COLS-1] == 0);
    assert(t1[ROWS/2][COLS/2] == 0);
    
    /* Check if probability of tree growth is within limits */
    for(i = 0; i < (G*10); i++){
        if(probGrowth()){
            growthCounter++;
        }
    }
    assert((growthCounter >= 3) && (growthCounter <= 18));
    
    /* Check if probability of tree burning is within limits */
    for(i = 0; i < (L*10); i++){
        if(probBurn()){
            burnCounter++;
        }
    }
    assert((burnCounter >= 3) && (burnCounter <= 18));
    
    /* Check cell booleans: 0 - Empty, 1 - Tree, 2 - Fire */
    assert(isEmpty(0) == true);
    assert(isTree(1) == true);
    assert(isFire(2) == true);
    
    /* Test 'safe' function at boundaries */
    assert(safe(ROWS, COLS) == false);
    assert(safe(ROWS-1, COLS-1) == true);
    assert(safe(ROWS/2, -1) == false);
    assert(safe(-1, COLS/2) == false);
    assert(safe(0,0) == true);
    assert(safe(-1,-1) == false);
     
    /* Test 'nextGen' and 'changeState' functions */
    initGrid(t2);
    initGrid(t3);
    
    /* Corner nodes are fires */
    t2[0][0] = 2;
    t2[ROWS-1][0] = 2;
    t2[ROWS-1][COLS-1] = 2;
    t2[0][COLS-1] = 2;
    
    /* Nodes around corner nodes are trees */
    t2[0][1] = 1;
    t2[1][0] = 1;
    t2[1][1] = 1;
    t2[ROWS-2][0] = 1;
    t2[ROWS-1][1] = 1;
    t2[ROWS-2][1] = 1;
    t2[ROWS-2][COLS-2] = 1;
    t2[ROWS-2][COLS-1] = 1;
    t2[ROWS-1][COLS-2] = 1;
    t2[0][COLS-2] = 1;
    t2[1][COLS-1] = 1;
    t2[1][COLS-2] = 1;
    
    /* Central node is a fire and nodes around it are trees */
    for(y = (ROWS/2)-1; y <= (ROWS/2)+1; y++){
        for(x = (COLS/2)-1; x <= (COLS/2)+1; x++){
            t2[y][x] = 1;
        }
    }
    t2[ROWS/2][COLS/2] = 2;
    
    /* Advance generation */
    nextGen(t2, t3);
    
    /* Assert expected fires in next generation t3 */
    assert(isFire(t3[0][1]));
    assert(isFire(t3[1][0]));
    assert(isFire(t3[1][1]));
    assert(isFire(t3[ROWS-2][0]));
    assert(isFire(t3[ROWS-1][1]));
    assert(isFire(t3[ROWS-2][1]));
    assert(isFire(t3[ROWS-2][COLS-2]));
    assert(isFire(t3[ROWS-2][COLS-1]));
    assert(isFire(t3[ROWS-1][COLS-2]));
    assert(isFire(t3[0][COLS-2]));
    assert(isFire(t3[1][COLS-1]));
    assert(isFire(t3[1][COLS-2]));
    assert(isFire(t3[(ROWS/2)-1][(COLS/2)-1]));
    assert(isFire(t3[(ROWS/2)-1][(COLS/2)]));
    assert(isFire(t3[(ROWS/2)-1][(COLS/2)+1]));
    assert(isFire(t3[(ROWS/2)][(COLS/2)-1]));
    assert(isFire(t3[(ROWS/2)][(COLS/2)+1]));
    assert(isFire(t3[(ROWS/2)+1][(COLS/2)-1]));
    assert(isFire(t3[(ROWS/2)+1][(COLS/2)]));
    assert(isFire(t3[(ROWS/2)+1][(COLS/2)+1]));
    
    /* Assert expected empty spaces */
    assert(isEmpty(t3[(ROWS/2)][(COLS/2)]));
    assert(isEmpty(t3[0][0]));
    assert(isEmpty(t3[ROWS-1][0]));
    assert(isEmpty(t3[ROWS-1][COLS-1]));
    assert(isEmpty(t3[0][COLS-1]));
    
    /* Test copyGrid function */
    initGrid(t4);
    t4[0][0] = 1;
    t4[ROWS/2][COLS/2] = 1;
    t4[(ROWS/2)-1][(COLS/2)-1] = 1;
    initGrid(t4_cp);
    copyGrid(t4, t4_cp);
    
    for(y = 0; y < ROWS; y++){
        for(x = 0; x < COLS; x++){
            if(t4_cp[y][x] != t4[y][x]){
                copy_flag = false;
            }
        }
    }
    assert(copy_flag = true);
}

/* Assigns value 0 to all elements in an initialised 2D grid */
void initGrid(int grid[ROWS][COLS])
{
    int i, j;
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLS; j++){
            grid[i][j] = 0;
        }
    }
}

/* Prints a 2D grid to the screen */
void printGrid(const int grid[ROWS][COLS])
{
    int i, j;
    printf("\n\n");
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLS; j++){
            if(isEmpty(grid[i][j])){
                neillfgcol(white);
                printf(" ");
            }
            if(isTree(grid[i][j])){
                neillfgcol(green);
                printf("@");
            }
            if(isFire(grid[i][j])){
                neillfgcol(red);
                printf("*");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

/* Tells a function whether to grow a tree for a 1 in G chance */
bool probGrowth(void)
{
    int k;
    k = (rand() % G) + 1;
    if(k == 1){
        return true;
    }
    return false;
}

/* Tells a function whether to burn a tree for a 1 in L chance */
bool probBurn(void)
{
    int k;
    k = (rand() % L) + 1;
    if(k == 1){
        return true;
    }
    return false;
}

/* Advances the grid by one generation */
void nextGen(const int old_grid[ROWS][COLS], int new_grid[ROWS][COLS])
{
    int i, j;
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLS; j++){
            new_grid[i][j] = changeState(old_grid, i, j);
        }
    }
}

/* Iterates through the grid and applies the change in states */
int changeState(const int grid[ROWS][COLS], int i, int j)
{
    int x, y;

    if(isFire(grid[i][j])){
        return 0;
    }
    
    if(isEmpty(grid[i][j])){
        if(probGrowth()){
            return 1;
        }
    }
    
    if(isTree(grid[i][j])){
        if(probBurn()){
            return 2;
        }
    }
    
    /* Check neighbouring 8 cells for fires ignoring cells
       out of bounds of the array */
    
    if(isTree(grid[i][j])){
        for(y = - 1; y <= 1; y++){
            for(x = -1; x <= 1; x++){
                if(safe((y+i), (x+j)))
                    if(isFire(grid[y+i][x+j])){
                        return 2;
                }
            }
        }
    }
    return grid[i][j];
}

/* Returns true if a grid element is empty */
bool isEmpty(const int state)
{
    if(state == 0){
        return true;
    }
    return false;
}

/* Returns true if a grid element is a tree */
bool isTree(const int state)
{
    if(state == 1){
        return true;
    }
    return false;
}

/* Returns true if a grid element is a fire */
bool isFire(const int state)
{
    if(state == 2){
        return true;
    }
    return false;
}

/* Ensures that only elements within array bounds are accessed */
bool safe(const int y, const int x)
{
    if( (y < 0) || (y >= ROWS) || (x < 0) || (x >= COLS) ){
        return false;
    }
    return true;
}

/* Makes a copy of a grid */
void copyGrid(const int src_grid[ROWS][COLS], int dst_grid[ROWS][COLS])
{
    int i, j;
    
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLS; j++){
            dst_grid[i][j] = src_grid[i][j];
        }
    }
}

/* Animates the grid when printing to terminal */
void animateGrid(const int grid[ROWS][COLS], const double delay)
{
    neillcursorhome();
    printGrid(grid);
    neillbusywait(delay);
}
