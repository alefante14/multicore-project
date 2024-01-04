//gcc  −g  −Wall  −fopenmp  −o
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

static int COL_DIMENSION = 9;

    //ogni valore è all'interno della matrice tramite i puntatori(vicini) e le sue ancore (che sono indice riga-colonna) 
    typedef struct matrix_value
    {
        //ancore
        int anchorX;
        int anchorY;
        //vicini
        int L;
        int R;
        int T;
        int B;   
    } matrix_value;
    
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

//restituisce il numero di elementi dentro una colonna
int column_size(int *column, int index){
    int n_elements = column[index];
    int size = 0;
    while (size!=n_elements){
        size++;
    }
    return size;
}

void column_create(int *column){
    for(int i=0; i<COL_DIMENSION; i++){
        column[i] = rand() % 9 + 1;
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    if(argc<2){
        printf("numero threads non specificati\n");
        exit(EXIT_FAILURE);
    }
    int thread_count = strtol(argv[1],NULL,10);

    double start;
    double end;

    //preparo la colonna
    int column[COL_DIMENSION];
    column_create(column);

    start = omp_get_wtime();

    //cerco la colonna con meno entry
    int current = 0;
    int smallest_col = current;
    int smallest_col_size = column_size(column,0);
    int col_size = COL_DIMENSION; //massimo numero di colonne
    
    int local_smallest_col_size;
    int local_smallest_col;
    //cerco la colonna con meno entry
    #pragma omp parallel num_threads(thread_count) private(local_smallest_col_size,local_smallest_col) shared(column)
    {
    local_smallest_col_size = smallest_col_size;
    local_smallest_col = smallest_col;
    #pragma omp for
    for(current = 1; current< col_size; current++){
        int check = column_size(column,current);
        if(check<local_smallest_col_size){
            local_smallest_col = current;
            local_smallest_col_size = check;
        } 
    }
    #pragma omp critical 
    {
    if (local_smallest_col_size < smallest_col_size) {
        smallest_col_size = local_smallest_col_size;
        smallest_col = local_smallest_col;
    }
    }
    }
    end = omp_get_wtime();
    printf("%f\n", end-start);

    //printf("index=%d, value=%d\n",smallest_col,smallest_col_size);
    //printf("Elapsed: %f seconds in the multithreading section\n",end-start);








    //SERIAL
    start = omp_get_wtime();

    current = 0;
    smallest_col = current;
    smallest_col_size = column_size(column,0);
    col_size = COL_DIMENSION; //massimo numero di colonne
    //cerco la colonna con meno entry
    while (current!=col_size){
        int check = column_size(column,current);
        if(check<smallest_col_size){
            smallest_col = current;
            smallest_col_size = check;
        }
        current++;
    }
    end = omp_get_wtime();
    //printf("index=%d, value=%d\n",smallest_col,smallest_col_size);
    //printf("Elapsed: %f seconds in the serial section\n",end-start);
    return 0;
}

