//gcc  −g  −Wall  −fopenmp  −o
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

static int DIMENSION = 9;
static int sudoku[9][9];


int main(int argc, char* argv[]){
    for(int x=0; x < DIMENSION; x++){
        for(int y=0; y < DIMENSION; y++){
            if(x==y) sudoku[x][y] = 1;
            else sudoku[x][y] = 0;
        }
    }
    if(argc<2){
        printf("numero threads non specificati\n");
        exit(EXIT_FAILURE);
    }
    int thread_count = strtol(argv[1],NULL,10);
    int main_matrix_dim=0; 

    double start;
    double end; 
    start = omp_get_wtime();
    #pragma omp parallel for num_threads(thread_count) collapse(2) schedule(static) \
        reduction(+: main_matrix_dim)
    for(int x=0; x < DIMENSION; x++){
        for(int y=0; y < DIMENSION; y++){
            if(sudoku[x][y]!=0){
                main_matrix_dim+=1;
            }
            else{
                main_matrix_dim+=2;
            }
        }
    } 
    end = omp_get_wtime();
    //printf("Elapsed: %f seconds in the multithreading section\n", end-start);
    printf("%f\n", end-start);

    //serial
    main_matrix_dim=0; 
    start = omp_get_wtime();
    for(int x=0; x < DIMENSION; x++){
        for(int y=0; y < DIMENSION; y++){
            if(sudoku[x][y]!=0){
                main_matrix_dim+=1;
            }
            else{
                main_matrix_dim+=2;
            }
        }
    } 
    end = omp_get_wtime();
    //printf("Elapsed: %f seconds in the serial section\n",end-start);
    //printf("%f\n", end-start);
    return 0;
}

