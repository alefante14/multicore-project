#include <stdio.h>
#include "sudoku.h"
#include "sudoku_math.h"
#define COOLING_RATE 0.99

/*
void rand_coord(int *x, int *y)
{
    *x = rand() % (SUDOKU_SIZE / SUDOKU_SQUARE);
    *y = rand() % (SUDOKU_SIZE / SUDOKU_SQUARE);
}
*/

void simulated_annealing(int sstate, int k_max)
{
    int cstate = sstate;
    int nstate;
    int temp;

    //Initial temp based on numbers of duplicates -> standard deviation of 10 attempts at randomly swapping two numbers
    //https://github.com/challengingLuck/youtube/blob/8ce9678db35ebfe4ac24af3450c3cbe313bc0215/sudoku/sudoku.py#L94
    //Iterations = number of non-fixed digits
    //cooling rate 0.99
    for (int i = 0; i < k_max; i++)
    {
        temp = i / k_max;
        //nstate =
    }
}

float get_sigma(int pscore[SUDOKU_SIZE * 2], int psudoku[SUDOKU_SIZE][SUDOKU_SIZE],int static_sudoku[SUDOKU_SIZE][SUDOKU_SIZE],Swappable sudoku_swappable[SUDOKU_SIZE])
{
    int temp_sudoku[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(temp_sudoku, psudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    int temp_score[SUDOKU_SIZE * 2];
    memcpy(temp_score, pscore, sizeof(int) * SUDOKU_SIZE * 2);

    int temp;
    float n_score[ANNEAL_TEMP_SAMPLE] = {0};
    int xa, ya, xb, yb;
    //printf("%d\n", pscore[11]);
    for (int attempt = 0; attempt < ANNEAL_TEMP_SAMPLE; attempt++)
    {
        random_positions(&xa, &ya, &xb, &yb,sudoku_swappable);
        swap_sudoku(xa, ya, xb, yb, temp_sudoku);
        update_score((int *)&temp_score, temp_sudoku, xa, ya, xb, yb);
        n_score[attempt] += (float)total_score((int *)&temp_score);
    }
    //printf("nscore 0 %f \n", n_score[0]);
    return calculateSD(n_score);
}
