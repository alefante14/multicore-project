#include "anneal.h"
#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


typedef struct thread_data{
    int (*sudoku)[SUDOKU_SIZE][SUDOKU_SIZE];
    int (*sudoku_static)[SUDOKU_SIZE][SUDOKU_SIZE];
    Swappable (*sudoku_swappable)[SUDOKU_SIZE];
    int *best_score;
    int *best_lock;
}Thread_data;



void thread_solve(void *input){
    srand(gettid());
    Thread_data *my_data = (Thread_data*) input;
    int (*sudoku)[SUDOKU_SIZE][SUDOKU_SIZE] = (*my_data).sudoku;
    int (*sudoku_static)[SUDOKU_SIZE][SUDOKU_SIZE] = (*my_data).sudoku_static;
    Swappable (*sudoku_swappable)[SUDOKU_SIZE] = (*my_data).sudoku_swappable;
    int *best_score = (*my_data).best_score;
    int *best_lock = (*my_data).best_lock;
        //printf("score > %d \n",*best_score);
    
    int iterations = SUDOKU_SIZE * SUDOKU_SIZE;
    int score[SUDOKU_SIZE * 2] = {0};
    init_score((int *)&score, sudoku);
    int cscore = total_score(score);
    float sigma = get_sigma(score, sudoku,sudoku_static,sudoku_swappable);
    int new_score;
    int attempts = 0;

    int temp_sudoku[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(temp_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    int temp_score[SUDOKU_SIZE * 2];
    memcpy(temp_score, score, sizeof(int) * SUDOKU_SIZE * 2);
    int xa, ya, xb, yb;
    

    while (*best_score != 0)
    {
        //printf("score > %d by %d\n",*best_score,gettid());
        for (int counter = 0; counter < iterations; counter++)
        {
            random_positions(&xa, &ya, &xb, &yb,sudoku_swappable);
            swap_sudoku(xa, ya, xb, yb, temp_sudoku);
            update_score((int *)&temp_score, temp_sudoku, xa, ya, xb, yb);
            new_score = total_score((int *)&temp_score);
            //printf("New score T:%d -> %d\n",gettid(),new_score);

            double rho = (exp(-(new_score - cscore) / sigma));
            int ext = (double)(rand() % SUDOKU_SQUARE);
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

            }
        }

        /*
        if (cscore <= 0)
        {
            printf("Score -> %d \n", cscore);
            print_sudoku(sudoku);
            break;
        }
        */


        if (new_score <= cscore)
        {
            cscore = new_score;
            memcpy(sudoku, temp_sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
            memcpy(score, temp_score, sizeof(int) * SUDOKU_SIZE * 2);

            
            if(cscore<*best_score){
                //printf("Attempt at writing from %d\n",gettid());
                while(*best_lock != 0){
                //Let's just wait for a second, shall we?
                //printf("[%d]> :(\n",gettid());
                }
                if(*best_lock == 0){
                    *best_lock = 1;
                }  
                if(cscore<*best_score){
                    *best_score = cscore;
                    printf("Improvement by Thread %d! > %d \n",gettid(),cscore);
                    printf("\n");
                    print_sudoku(sudoku);
                    printf("\n");
                }
                //printf("[%d]> :)\n",gettid());
                *best_lock = 0;
            }
        }

        else 
        {
            memcpy(temp_sudoku, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
            memcpy(temp_score, score, sizeof(int) * SUDOKU_SIZE * 2);
            attempts++;
        }

        if (attempts > SUDOKU_SIZE * SUDOKU_SIZE)
        {
            sigma += 2.0;
        }
    }
    pthread_exit(0);
    //print_sudoku(sudoku);
}



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
    //srand(time(NULL));

    FILE *in_file = fopen(argv[2],"r");
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


    init_score((int *)&score, sudoku);
    int best_score = total_score(score);
    int best_lock = 0;


    memcpy(sudoku_static, sudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);
    populate_sudoku(sudoku,sudoku_swappable);
    print_sudoku(sudoku);



    struct thread_data thread_input;
    thread_input.sudoku = &sudoku;
    thread_input.sudoku_static = &sudoku_static;
    thread_input.sudoku_swappable = &sudoku_swappable;
    thread_input.best_score = &best_score;
    thread_input.best_lock = &best_lock;

    char *p;
    int thread_number = strtol(argv[1],&p,10);
    pthread_t *arr_thread;
    arr_thread = malloc(sizeof(pthread_t)*thread_number*thread_number);
    for(int threadn = 0; threadn<thread_number;threadn++){
        pthread_create(&arr_thread[threadn],NULL,thread_solve,(void*)&thread_input);
    }
    for(int threadn = 0; threadn<thread_number;threadn++){
        pthread_join(arr_thread[threadn],NULL);
    }

    /*
    pthread_t thread_id1;
    pthread_t thread_id2;
    pthread_t thread_id3;
    pthread_t thread_id4;


    pthread_create(&thread_id1,NULL,thread_solve,(void*)&thread_input);
    pthread_create(&thread_id2,NULL,thread_solve,(void*)&thread_input);
    pthread_create(&thread_id3,NULL,thread_solve,(void*)&thread_input);
    pthread_create(&thread_id4,NULL,thread_solve,(void*)&thread_input);

    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);
    pthread_join(thread_id3,NULL);
    pthread_join(thread_id4,NULL);
*/

    print_sudoku(sudoku);

}

//compile -> gcc *.c -lm -o Output
