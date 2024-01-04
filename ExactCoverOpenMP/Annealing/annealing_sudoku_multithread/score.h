    #include <stdio.h>

void init_score(int *pscore, int psudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
    int found = 0;
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {

            if (j > 0)
            {
                found = 0;
                for (int l = 0; l < j; l++)
                {
                    //printf("I:%d,J:%d\n", i, j);
                    //printf("psudoku[i][j] %d, psudoku[i][l] %d \n", psudoku[i][j], psudoku[i][l]);
                    if (psudoku[i][j] == psudoku[i][l] && found != 1)
                    {
                        pscore[i]++;
                        //printf("pscore[%d] -> %d \n", i, pscore[i]);
                        found = 1;
                    }
                }
            }

            if (i > 0)
            {
                found = 0;
                for (int n = 0; n < i; n++)
                {
                    //printf("I:%d,J:%d,N:%d\n", i, j, n);
                    //printf("psudoku[i][j] %d, psudoku[n][j] %d \n", psudoku[i][j], psudoku[n][j]);
                    if (psudoku[i][j] == psudoku[n][j] && found != 1)
                    {
                        pscore[j + SUDOKU_SIZE]++;
                        //printf("pscore[%d] -> %d \n", j + 9, pscore[j + 9]);
                        found = 1;
                    }
                }
            }
        }
    }
}

void update_score(int *pscore, int psudoku[SUDOKU_SIZE][SUDOKU_SIZE], int xa, int ya, int xb, int yb)
{
    //xa
    int found = 0;
    pscore[xa] = 0;
    for (int counter = 0; counter < SUDOKU_SIZE; counter++)
    {
        found = 0;
        for (int compare = 0; compare < counter; compare++)
        {
            if (psudoku[xa][counter] == psudoku[xa][compare] && found == 0)
            {
                //printf(":)\n");
                pscore[xa]++;
                found++;
            }
        }
    }
    /*
    if (xa != 8)
    {
        pscore[xa]--;
    }
    */
    //xb

    pscore[xb] = 0;
    for (int counter = 0; counter < SUDOKU_SIZE; counter++)
    {
        found = 0;
        for (int compare = 0; compare < counter; compare++)
        {
            if (psudoku[xb][counter] == psudoku[xb][compare] && counter!=compare)
            {
                //printf(":(\n");
                pscore[xb]++;
                found++;
            }
        }
    }
    /*
    if (xb != 8)
    {
        pscore[xb]--;
    }
    */
    //ya

    pscore[ya + SUDOKU_SIZE] = 0;
    //printf("ya - %d", ya);
    for (int counter = 0; counter < SUDOKU_SIZE; counter++)
    {
        found = 0;
        for (int compare = 0; compare < counter; compare++)
        {
            if (psudoku[counter][ya] == psudoku[compare][ya] && counter!=compare)
            {
                pscore[ya + SUDOKU_SIZE]++;

                //printf("sustown %d - %d\n", ya + 9, pscore[ya + 9]);
                found++;
            }
        }
    }
    //yb

    pscore[yb + SUDOKU_SIZE] = 0;
    //printf("ya - %d", ya);
    for (int counter = 0; counter < SUDOKU_SIZE; counter++)
    {
        found = 0;
        for (int compare = 0; compare < counter; compare++)
        {
            if (psudoku[counter][yb] == psudoku[compare][yb] && found == 0)
            {
                pscore[yb + SUDOKU_SIZE]++;

                //printf("sustown %d - %d\n", yb + 9, pscore[yb + 9]);
                found++;
            }
        }
    }
}

int total_score(int pscore[SUDOKU_SIZE*2])
{
    int sum = 0;

    for (int i = 0; i < (SUDOKU_SIZE * 2); i++)
    {
        //printf(">> %d - i%d \n", pscore[i], i);
        sum += pscore[i];
    }
    return sum;
}

/*
int total_sxscore(int pscore[SUDOKU_SIZE*2])
{
    int sum = 0;

    for (int i = 0; i < (SUDOKU_SIZE); i++)
    {
        //printf(">> %d - i%d \n", pscore[i], i);
        sum += pscore[i];
    }
    return sum;
}

int total_dxscore(int pscore[SUDOKU_SIZE*2])
{
    int sum = 0;

    for (int i = 0; i < (SUDOKU_SIZE); i++)
    {
        //printf(">> %d - i%d \n", pscore[i], i);
        sum += pscore[i+9];
    }
    return sum;
}
*/