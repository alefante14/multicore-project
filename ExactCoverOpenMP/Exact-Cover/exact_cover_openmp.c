#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "sudoku.h"
#include "stack.h"
#include "matrix.h"
//gcc -g -Wall -fopenmp -o ec_omp exact_cover_openmp.c

/*
    creo lo "scheletro" della matrice
            rows = RxCy#z (possible_value)
            columns = Constraints (RxCy,Rx#z,Cy#z,Bw#z) (constraint)
    step 2 : assign 0 or 1 in each cell of the matrix

    se la matrice non ha colonne, ferma l'algoritmo
    se una riga non ha almeno un 1, no soluzione, ferma algoritmo
    
    scegli una colonna c
    scorri ogni r che ha un 1 in c e prendi la r con il minor numero di 1 al suo interno
            considera ogni colonna c' che ha un 1 nella riga r
            rimuovi la colonna c'
            rimuovi ogni riga con un 1 in c'
        applica algoritmo alla nuova matrice

    dove per noi 1 = appartiene alla lista linkata, 0 = non appartiene alla lista linkata
    dancing links
    una lista di elementi linkati tra di loro
    a = elemento
    R[a] = elemento a destra di a
    L[a] = elemento a sinistra di a
    per eliminare elemento ->   R[L[a]] = R[a] ... L[R[a]] = L[a]
    per reinserirlo -> R[L[a]] = a ... L[R[a]] = a
*/

/*
    Il sudoku più difficile del mondo!
800000000003600000070090200050007000000045700000100030001000068008500010090000400
*/

//da invocare a fine algoritmo
void freeAll(step *all_steps, int *current_step, matrix_value *m, possible_value *poss, constraint *con){
    int current = 0;
    while (current < *current_step){
       freeArray(&all_steps[current].columns);
       freeArray(&all_steps[current].elements);
       current++;
    }
    free(m);
    free(poss);
    free(con);
}


//a partire dagli step, ricostruisce la matrice facendo backtracking
void rebuilder(matrix_value *main_matrix, possible_value *poss_val_arr, constraint *cons_arr,int *cons_arr_size, int *root,
 step *all_steps, int *current_step){
    bool checkSplit = false;
    bool end = false;
    while (end == false){
        while (checkSplit == false){
            if(all_steps[*current_step].isSplit)
                checkSplit = true;
            int elmt_to_rebuild = popArray(&all_steps[*current_step].columns);
            while (elmt_to_rebuild != -1){
                insert_column(cons_arr,elmt_to_rebuild,cons_arr_size,root);
                elmt_to_rebuild = popArray(&all_steps[*current_step].columns);
            }
            freeArray(&all_steps[*current_step].columns);
            elmt_to_rebuild = popArray(&all_steps[*current_step].elements);
            while (elmt_to_rebuild != -1){
                insert_element(main_matrix,elmt_to_rebuild,poss_val_arr,cons_arr);
                elmt_to_rebuild = popArray(&all_steps[*current_step].elements);
            }
            freeArray(&all_steps[*current_step].elements);
            *current_step-=1;
            //SE CURRENT_STEP == -1, SONO TORNATO INDIETRO ALL'ORIGINE, NO SOLUZIONE
            if(*current_step == -1){
                printf("non esiste una soluzione\n");
                freeAll(all_steps,current_step,main_matrix,poss_val_arr,cons_arr);
                exit(EXIT_FAILURE);
            }
        }
        if(main_matrix[all_steps[*current_step+1].currentX].B != -1){
            end = true;
            *current_step+=1;
        }
        else
            checkSplit =false;        
    }
    
}



int main(int argc, char* argv[]) {
    
    double start;
    double end; 
    start = omp_get_wtime();

    
    if(argc<2){
        printf("numero threads non specificati\n");
        exit(EXIT_FAILURE);
    }
    int thread_count = strtol(argv[1],NULL,10);

        FILE *puzzles;
    if((puzzles = fopen("./puzzles.txt","r")) == NULL){
        printf("file puzzles.txt non trovato!\n");
        exit(1);
    }
    //prendo il sudoku dal file
    int sudoku[DIMENSION*DIMENSION];
    sudoku_from_file(puzzles,sudoku);
    printf("sudoku da risolvere\n");
    print_sudoku_vector(sudoku);
    

    //alloco indice colonne di constraints
    //(RxCy,Rx#z,Cy#z,Bw#z)
    constraint *cons_arr; //colonne
    int cons_arr_dim = 4*DIMENSION*DIMENSION;
    cons_arr = malloc((cons_arr_dim)*sizeof(constraint));

    //alloco indice di righe RxCy#value
    possible_value *poss_val_arr; //righe
    int poss_val_dim = DIMENSION*DIMENSION*DIMENSION;
    poss_val_arr = malloc((poss_val_dim)*sizeof(possible_value));


    int x,y,i,const_type;
    //sto assegnando i valori ai vari indici riga/colonna che saranno costanti per ogni sudoku
    //GUARDARE FILE parallel.c
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
    cons_arr[cons_arr_dim-1].Rgt = -1;
    
    
    //alloco la matrice
    matrix_value *main_matrix;
   //la grandezza della matrice di calcolo varia in base al numero di caselle già riempite
   int main_matrix_dim=0; 
    #pragma omp parallel for num_threads(thread_count) collapse(2)  \
        reduction(+: main_matrix_dim)
    for(int x=0; x < DIMENSION; x++){
        for(int y=0; y < DIMENSION; y++){
            if(sudoku[DIMENSION*x + y]!=0){
                main_matrix_dim+=4;
            }
            else{
                main_matrix_dim+=4*DIMENSION;
            }
        }
    } 
    main_matrix = malloc((main_matrix_dim)*sizeof(matrix_value));

    //popolo la matrice   
    int current_matrix_pos = 0;
    for(int x=0; x < DIMENSION; x++){
        for(int y=0; y < DIMENSION; y++){
            //ogni indice della colonna contiene al suo interno l'ultimo elemento dall'alto della colonna
            if(sudoku[DIMENSION*x + y]!=0){
                //collegamenti orizzontali
                rows_linker(main_matrix,x,y,current_matrix_pos,sudoku[DIMENSION*x + y]-1,poss_val_arr);
                //collegamenti verticali
                column_top_linker(main_matrix,x,y,current_matrix_pos,sudoku[DIMENSION*x + y]-1,cons_arr);

                current_matrix_pos+=4; 
            }
            else{
                //se ho una casella vuota nel sudoku, ho 9 possibili valori invece che 1
                for (int poss_n = 0; poss_n < DIMENSION; poss_n++)
                {
                    rows_linker(main_matrix,x,y,current_matrix_pos,poss_n,poss_val_arr);
                    column_top_linker(main_matrix,x,y,current_matrix_pos,poss_n,cons_arr);
                    current_matrix_pos+=4;
                }
            }
        }
    }
    column_bottom_linker(main_matrix,cons_arr,cons_arr_dim);   
    //end populate matrix


    //solution contiene indice riga scelta come soluzione
    int solution[DIMENSION][DIMENSION];

    step all_steps[81];// = malloc(81*sizeof(step));
    int current_step = 0;
    int root = 0;

    while(cons_arr_dim>0){
        int current = root;
        while(current!=-1){
            int current_col_size = column_size(cons_arr,main_matrix,current);
            //cerca una colonna con un solo elemento
            if(current_col_size<=1){
                //se entra nell'if, siamo ad un vicolo cieco
                if(current_col_size==0){
                    //termina l'algoritmo
                    if(current_step == 0){
                        printf("non esiste una soluzione\n");
                        freeAll(all_steps,&current_step,main_matrix,poss_val_arr,cons_arr);
                        exit(EXIT_FAILURE);
                    }
                    //rebuilder torna a costruire fino al prossimo split
                    //se a quello split ha percorso già tutti i path, continua a ricostruire
                    current_step--;
                    rebuilder(main_matrix,poss_val_arr,cons_arr,&cons_arr_dim,&root,all_steps,&current_step);
                    //passo allo step split il prossimo elemento
                    int next_elmt = main_matrix[all_steps[current_step].currentX].B;
                    all_steps[current_step].currentX = next_elmt;
                    //forzo l'algoritmo a continuare con la riga di currentX
                    int row_selected = main_matrix[next_elmt].anchorX;
                    //riscrivo sopra solution
                    solution[poss_val_arr[row_selected].R][poss_val_arr[row_selected].C] = poss_val_arr[row_selected].value+1;
                    remove_columns_from_row(main_matrix,row_selected,poss_val_arr,
                     cons_arr,&poss_val_dim,&cons_arr_dim, &root,all_steps,current_step,false);
                    current_step++;
                    break;
                }

                //aggiungo la riga scelta alla soluzione
                int row_selected = main_matrix[cons_arr[current].column_start].anchorX;
                solution[poss_val_arr[row_selected].R][poss_val_arr[row_selected].C] = poss_val_arr[row_selected].value+1;
                //rimuovo le colonne a partire dalla riga selezionata
                remove_columns_from_row(main_matrix,row_selected,poss_val_arr,
                 cons_arr,&poss_val_dim,&cons_arr_dim, &root,all_steps,current_step,false);
                current_step++;
                break;
            }
            
            current = cons_arr[current].Rgt;
        }

        //non ha trovato colonne con un solo elemento
        //gesisce solo lo start del primo spit
        if(current == -1){
            //printf("SPLIT at step %d\n",current_step);
            int smallest_column = smallest_column_finder(cons_arr,main_matrix,root);
            int row_selected = main_matrix[cons_arr[smallest_column].column_start].anchorX;
            solution[poss_val_arr[row_selected].R][poss_val_arr[row_selected].C] = poss_val_arr[row_selected].value+1;
            all_steps[current_step].isSplit = true;
            all_steps[current_step].currentX = cons_arr[smallest_column].column_start;
            //rimuovo le colonne a partire dalla riga selezionata
            remove_columns_from_row(main_matrix,row_selected,poss_val_arr,
            cons_arr,&poss_val_dim,&cons_arr_dim, &root,all_steps,current_step, true);
            current_step++;
        }
        
    }    
    //printf("current step %d\n",current_step);
    end = omp_get_wtime();
    printf("soluzione\n");
    print_sudoku(solution);
    freeAll(all_steps,&current_step,main_matrix,poss_val_arr,cons_arr);
    printf("Elapsed: %f seconds in the main algorithm\n", end-start);
}