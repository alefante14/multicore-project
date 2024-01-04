#include "anneal.h"
#include "score.h"

void main(int argc, char **argv)
{
    int sudoku[SUDOKU_SIZE][SUDOKU_SIZE] = {0};
    int sudoku_static[SUDOKU_SIZE][SUDOKU_SIZE];
    int score[SUDOKU_SIZE * 2] = {0};
    int iterations = SUDOKU_SIZE * SUDOKU_SIZE;
    Swappable sudoku_swappable[SUDOKU_SIZE];
    for(int i = 0; i<SUDOKU_SIZE;i++){
        sudoku_swappable[i].size=0;
        for (int j = 0; j<SUDOKU_SIZE;j++){
            sudoku_swappable[i].value[j] = 0;
        }
    }
    srand(time(NULL));
    /*
    for (int x = 0; x < SUDOKU_SIZE; x++)
    {
        for (int y = 0; y < SUDOKU_SIZE; y++)
        {
            if (rand() % 9 == 0)
            {
                iterations--;
                sudoku[x][y] = rand() % 9 + 1;
            }
        }
    }
    */

  /*
    sudoku[0][0] = 8;
    sudoku[1][2] = 3;
    sudoku[1][3] = 6;
    sudoku[2][1] = 7;
    sudoku[2][4] = 9;
    sudoku[2][6] = 2;
    sudoku[4][4] = 4;
    sudoku[4][5] = 5;
    sudoku[4][6] = 7;
    sudoku[5][3] = 1;
    sudoku[5][7] = 3;
    sudoku[6][7] = 6;
    sudoku[6][8] = 8;
    sudoku[7][2] = 8; 
    sudoku[7][3] = 5;
    sudoku[8][6] = 4;
    */
    
    
    FILE *in_file = fopen(argv[1],"r");


    memcpy(sudoku_static, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);


    int tsudoku[SUDOKU_SIZE*SUDOKU_SIZE] = {0};
    sudoku_from_file(in_file, tsudoku);
    int mcounter = 0;
    for(int i = 0; i<SUDOKU_SIZE;i++){
            for(int j = 0; j<SUDOKU_SIZE;j++){
                //printf("%d",tsudoku[mcounter]);
                sudoku[i][j] = tsudoku[mcounter];
                mcounter++;
        }
    }
    memcpy(sudoku_static, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    populate_sudoku(sudoku,sudoku_swappable);

    init_score((int *)&score, sudoku);
    //printf("score 9 %d\n", score[9]);
    //printf("score 11 %d\n", score[11]);
    int cscore = total_score(score);
    //printf("total score -> %d\n", cscore);
    print_sudoku(sudoku);
    float sigma = get_sigma(score, sudoku,sudoku_static,sudoku_swappable);
    //printf("Sigma: %f\n", sigma);

    int new_score;
    //int sx_score;
    //int dx_score;
    int attempts = 0;

    int temp_sudoku[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(temp_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    int temp_score[SUDOKU_SIZE * 2];
    memcpy(temp_score, score, sizeof(int) * SUDOKU_SIZE * 2);
    int xa, ya, xb, yb;

    int backup_sudoku[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(backup_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    int backup_score[SUDOKU_SIZE * 2];
    memcpy(backup_score, score, sizeof(int) * SUDOKU_SIZE * 2);

    while (score != 0)
    {
        for (int counter = 0; counter < iterations; counter++)
        {
            random_positions(&xa, &ya, &xb, &yb, sudoku_swappable);
            //printf("??%d %d  -  %d %d\n", xa, ya, xb, yb);
            swap_sudoku(xa, ya, xb, yb, temp_sudoku);
            update_score((int *)&temp_score, temp_sudoku, xa, ya, xb, yb);
            new_score = total_score((int *)&temp_score);
            /*
            sx_score = total_sxscore((int *)&temp_score);
            dx_score = total_dxscore((int *)&temp_score);
            
            //printf("%d ",sx_score);
            printf("%d \n",dx_score);
            */

            double rho = (exp(-(new_score - cscore) / sigma));
            int ext = (double)(rand() % 3);
            if (ext == 2)
            {
                ext--;
            }
            sigma *= COOLING_RATE;

            if (!(ext < rho))
            {
                memcpy(temp_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
                memcpy(temp_score, score, sizeof(int) * SUDOKU_SIZE * 2);
            }
            else
            {

                //printf("tempscore -> %d, sigma -> %f\n", new_score, sigma);
            }
        }

        if (cscore <= 0)
        {
            //printf("Score -> %d \n", cscore);
            print_sudoku(sudoku);
            break;
        }/*
        else if(sx_score == 0){

        }
        else if(sx_score == 0){

        }
        */
        else
        {
            if(new_score < cscore){
                printf("Score -> %d \n", cscore);
                print_sudoku(sudoku);
                for (int tt = 0; tt < 18; tt++)
                {
                    printf("[%d]", score[tt]);
                }
                printf("\n");
            }
            if (new_score <= cscore)
            {
                cscore = new_score;
                memcpy(sudoku, temp_sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
                memcpy(score, temp_score, sizeof(int) * SUDOKU_SIZE * 2);
                
                
            }

            else
            {
                //printf("fail.. %d\n", new_score);
                //print_sudoku(temp_sudoku);
                memcpy(temp_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
                memcpy(temp_score, score, sizeof(int) * SUDOKU_SIZE * 2);
                attempts++;
            }
        }

        if (attempts > SUDOKU_SIZE * SUDOKU_SIZE)
        {
            //printf("sus\n");
            //printf("score -> %d, sigma -> %f\n", cscore, sigma);
            sigma += 2.0;
        }
    }
}

//compile -> gcc *.c -lm -o Output
