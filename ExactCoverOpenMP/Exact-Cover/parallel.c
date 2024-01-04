//gcc −fopenmp  −o  parallel  parallel.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

//colonne della matrice, chi fa parte di una colonna ne rispetta i constraint (è una lista linkata anche questa)
typedef struct constraint{
    int R; //row
    int C; //column
    int B; //box
    int value;
    //vicini
    int Lft;
    int Rgt;

    int column_start;
    int column_last_element;
} constraint;

//righe della matrice, ogni riga indica un possibile value che può essere assunto nella casella del sudoku R,C
typedef struct possible_value
{
    int R;  //row
    int C;  //column
    int value;
    int row_start;
} possible_value;


static int DIMENSION = 9;
static int sudoku[9][9];
static possible_value poss_val_arr[9*9*9];
static constraint cons_arr[4*9*9];


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

    int x,y,i,const_type;
    double start;
    double end; 
    start = omp_get_wtime();
    //sto assegnando i valori ai vari indici riga/colonna che saranno costanti per ogni sudoku
    #pragma omp parallel for schedule(static,4) num_threads(thread_count) \
    private(x,y,i,const_type) shared(poss_val_arr,cons_arr) collapse(3)
    for(const_type=0; const_type < 4; const_type++){
        for(x=0; x < DIMENSION; x++){
            for(y=0; y < DIMENSION; y++){
                for(i=0; i < DIMENSION; i++){
                    //popolo indice righe
                    int current_poss_val = x*DIMENSION*DIMENSION + y*DIMENSION + i;
                    poss_val_arr[current_poss_val].R = x;
                    poss_val_arr[current_poss_val].C = y;
                    poss_val_arr[current_poss_val].value = i;
                }
                //popolo indice colonne
                int current_cons_arr = const_type*DIMENSION*DIMENSION + x*DIMENSION + y;
                cons_arr[current_cons_arr].column_start = -1;
                cons_arr[current_cons_arr].column_last_element = -1;
                cons_arr[current_cons_arr].Lft = current_cons_arr - 1;
                cons_arr[current_cons_arr].Rgt = current_cons_arr + 1;
                switch (const_type)
                {
                case 0:
                    cons_arr[current_cons_arr].R = x;
                    cons_arr[current_cons_arr].C = y;
                    cons_arr[current_cons_arr].B = -1;
                    cons_arr[current_cons_arr].value = -1;
              
                break;
                case 1:
                    cons_arr[current_cons_arr].R = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].C = -1;
                    cons_arr[current_cons_arr].B = -1;

                break;
                case 2:
                    cons_arr[current_cons_arr].C = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].R = -1;
                    cons_arr[current_cons_arr].B = -1;

                break;
                case 3:
                    cons_arr[current_cons_arr].B = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].C = -1;
                    cons_arr[current_cons_arr].R = -1;

                default:
                    break;
                }
            }            
        }
    }

    cons_arr[0].Lft=-1;
    cons_arr[4*DIMENSION*DIMENSION-1].Rgt = -1;
    end = omp_get_wtime();
    //printf("Elapsed: %f seconds in the multithreading section\n", end-start);
    printf("%f\n", end-start);



    //SERIAL
    start = omp_get_wtime();
    //sto assegnando i valori ai vari indici riga/colonna che saranno costanti per ogni sudoku
    for(const_type=0; const_type < 4; const_type++){
        for(x=0; x < DIMENSION; x++){
            for(y=0; y < DIMENSION; y++){
                for(i=0; i < DIMENSION; i++){
                    //popolo indice righe
                    int current_poss_val = x*DIMENSION*DIMENSION + y*DIMENSION + i;
                    poss_val_arr[current_poss_val].R = x;
                    poss_val_arr[current_poss_val].C = y;
                    poss_val_arr[current_poss_val].value = i;
                }
                //popolo indice colonne
                int current_cons_arr = const_type*DIMENSION*DIMENSION + x*DIMENSION + y;
                cons_arr[current_cons_arr].column_start = -1;
                cons_arr[current_cons_arr].column_last_element = -1;
                cons_arr[current_cons_arr].Lft = current_cons_arr - 1;
                cons_arr[current_cons_arr].Rgt = current_cons_arr + 1;
                switch (const_type)
                {
                case 0:
                    cons_arr[current_cons_arr].R = x;
                    cons_arr[current_cons_arr].C = y;
                    cons_arr[current_cons_arr].B = -1;
                    cons_arr[current_cons_arr].value = -1;
              
                break;
                case 1:
                    cons_arr[current_cons_arr].R = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].C = -1;
                    cons_arr[current_cons_arr].B = -1;

                break;
                case 2:
                    cons_arr[current_cons_arr].C = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].R = -1;
                    cons_arr[current_cons_arr].B = -1;

                break;
                case 3:
                    cons_arr[current_cons_arr].B = x;
                    cons_arr[current_cons_arr].value = y;
                    cons_arr[current_cons_arr].C = -1;
                    cons_arr[current_cons_arr].R = -1;

                default:
                    break;
                }
            }            
        }
    }

    cons_arr[0].Lft=-1;
    cons_arr[4*DIMENSION*DIMENSION-1].Rgt = -1;
    end = omp_get_wtime();
    //printf("Elapsed: %f seconds in the serial section\n", end-start);
    printf("%f\n", end-start);

}

