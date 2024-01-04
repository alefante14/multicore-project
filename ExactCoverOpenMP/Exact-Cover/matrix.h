#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

//SEPARARE STEP DAL RESTO

    static int DIMENSION = 9;

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

    //righe della matrice, ogni riga indica un possibile value che può essere assunto nella casella del sudoku R,C
    typedef struct possible_value
    {
        int R;  //row
        int C;  //column
        int value;
        int row_start;
    } possible_value;

    //usato per tenere traccia delle modifiche effettuate dall'algoritmo alla matrice 
    typedef struct step
    {
        arraylist columns;
        arraylist elements;
        bool isSplit;
        int currentX; //è l'elemento selezionato  
    } step;


//dando in input le caratteristiche dell'indice colonna, ci restituisce il numero dell'indice
int constraint_finder(int type, int R, int C, int B, int value){
    switch (type)
    {
    case 0:
        return R*DIMENSION + C;
    case 1:
        return type*DIMENSION*DIMENSION + R*DIMENSION + value;
    case 2:
        return type*DIMENSION*DIMENSION + C*DIMENSION + value;
    case 3:
        return type*DIMENSION*DIMENSION + B*DIMENSION + value;
    default:
        return -1;
    }
}


//restituisce il numero di elementi dentro una colonna
int column_size(constraint *cons_arr,matrix_value *main_matrix, int c){
    int current = cons_arr[c].column_start;
    int size = 0;
    while (current!=-1){
        size++;
        current = main_matrix[current].B;
    }
    return size;
}

//trova la colonna più piccola
int smallest_column_finder(constraint *cons_arr, matrix_value *main_matrix, int root){
    int current = root;
    int smallest_col = current;
    int smallest_col_size = column_size(cons_arr,main_matrix,current);
    current = cons_arr[current].Rgt;
    //cerco la colonna con meno entry
    while (current!=-1){
        int check = column_size(cons_arr,main_matrix,current);
        if(check<smallest_col_size){
            smallest_col = current;
            smallest_col_size = check;
        }
        current = cons_arr[current].Rgt;
    }
    return smallest_col;
}


//LINKER

//collega orizzontalmente gli elementi della matrice e aggancia all'indice orizzontale
void rows_linker(matrix_value *main_matrix,int x, int y, int curr_pos, int value, possible_value *poss_val_arr){
    //aggiorno link array righe e riferimento della matrix
    poss_val_arr[x*DIMENSION*DIMENSION + y*DIMENSION + value].row_start = curr_pos;
    for(int i=0; i < 4; i++){
        main_matrix[curr_pos+i].anchorX = x*DIMENSION*DIMENSION + y*DIMENSION + value; 
        //collegamenti orizzontali
        if(i!=0)
            main_matrix[curr_pos+i].L = curr_pos+i-1;
        else
            main_matrix[curr_pos+i].L = -1; //primo elemento a sinistra
        if(i!=3)
            main_matrix[curr_pos+i].R = curr_pos+i+1;
        else
            main_matrix[curr_pos+i].R = -1; //ultimo elemento a destra
    }
}

//collega verticalmente gli elementi della matrice dall'alto verso il basso
void column_top_linker(matrix_value *main_matrix, int x, int y, int curr_pos,int value,constraint *cons_arr){
    for(int i=0; i < 4; i++){
        int elmt_column_inx =  constraint_finder(i,x,y,box_conv(x,y),value);
        int col_last_element = cons_arr[elmt_column_inx].column_last_element;
        main_matrix[curr_pos+i].anchorY = elmt_column_inx;
        //primo elemento della colonna
        if(col_last_element==-1){
            main_matrix[curr_pos+i].T = -1;
            cons_arr[elmt_column_inx].column_last_element = curr_pos+i;
            cons_arr[elmt_column_inx].column_start = curr_pos+i;
        }
        else{
            main_matrix[curr_pos+i].T = col_last_element;
            cons_arr[elmt_column_inx].column_last_element = curr_pos+i;
        }
    }
}

//collega verticalmente gli elementi della matrice dal basso verso l'alto 
//(da invocare quando ho inserito tutti gli elementi)
void column_bottom_linker(matrix_value *main_matrix,constraint *cons_arr, int cons_arr_dim){
    for(int current = 0; current < cons_arr_dim; current++){
        int bottom = cons_arr[current].column_last_element;
        int top = cons_arr[current].column_start;
        main_matrix[bottom].B = -1;
        while(bottom!=top){
            int above = main_matrix[bottom].T;
            main_matrix[above].B = bottom;
            bottom = main_matrix[bottom].T;
        }
    }
}


//MODIFICHE ALLA MATRICE

//permette di rimuovere una colonna nella matrice
void remove_column(constraint *cons_arr, int n, int *size, int *root){
    //rimasto ultimo elmt
    if(*root==n && cons_arr[n].Rgt==-1){
        *root = -1;
    }
    //primo elmt
    else if(*root==n){
        *root = cons_arr[n].Rgt; 
        cons_arr[cons_arr[n].Rgt].Lft = cons_arr[n].Lft;
    }
    //ultimo elmt
    else if(cons_arr[n].Rgt==-1){
        cons_arr[cons_arr[n].Lft].Rgt = cons_arr[n].Rgt;
    }
    else{
        cons_arr[cons_arr[n].Lft].Rgt = cons_arr[n].Rgt;
        cons_arr[cons_arr[n].Rgt].Lft = cons_arr[n].Lft;
    }
    *size -= 1;
}


//permette di inserire una colonna nella matrice
void insert_column(constraint *cons_arr, int n, int *size, int *root){
    //se l'inizio è a destra
    if(*root==cons_arr[n].Rgt){
        *root = n;
        cons_arr[cons_arr[n].Rgt].Lft = n;
    }
    //se inserisco ultimo elemento
    else if(cons_arr[cons_arr[n].Lft].Rgt==-1){
        cons_arr[cons_arr[n].Lft].Rgt = n;
    }
    else{
        cons_arr[cons_arr[n].Lft].Rgt = n;
        cons_arr[cons_arr[n].Rgt].Lft = n;
    }
    *size += 1;
}


//rimuove un elemento nella matrice
void remove_element(matrix_value *main_matrix, int n, possible_value *poss_val_arr, constraint *cons_arr){
    int anchorX = main_matrix[n].anchorX;
    //caso in cui è il primo della riga
    if(poss_val_arr[anchorX].row_start == n){
        poss_val_arr[anchorX].row_start = main_matrix[n].R;
        //se non è l'ultimo elemento
        if(main_matrix[n].R!=-1)
            main_matrix[main_matrix[n].R].L = main_matrix[n].L;
    }
    else{
        main_matrix[main_matrix[n].L].R = main_matrix[n].R;
        main_matrix[main_matrix[n].R].L = main_matrix[n].L;
    }


    int anchorY = main_matrix[n].anchorY;
    //caso in cui è il primo della colonna
    if(cons_arr[anchorY].column_start == n){
        cons_arr[anchorY].column_start = main_matrix[n].B;
        //se non è l'ultimo elemento
        if(main_matrix[n].B!=-1)
            main_matrix[main_matrix[n].B].T = main_matrix[n].T;
    }
    //se è l'ultimo elemento, ma non è il primo
    else if(cons_arr[anchorY].column_last_element == n){
        cons_arr[anchorY].column_last_element = main_matrix[n].T;
        main_matrix[main_matrix[n].T].B = main_matrix[n].B; 
    }
    else{
        main_matrix[main_matrix[n].T].B = main_matrix[n].B;
        main_matrix[main_matrix[n].B].T = main_matrix[n].T;
    }

}

//inserisce un elemento nella matrice
void insert_element(matrix_value *main_matrix, int n, possible_value *poss_val_arr, constraint *cons_arr){
    int anchorX = main_matrix[n].anchorX;
    //se diventa il primo della riga
    if(poss_val_arr[anchorX].row_start == main_matrix[n].R){
        poss_val_arr[anchorX].row_start = n;
        main_matrix[n].L = -1;
        //se la riga non è vuota
        if(main_matrix[n].R!=-1)
            main_matrix[main_matrix[n].R].L = n;
    }
    else{
        main_matrix[main_matrix[n].L].R = n;
        main_matrix[main_matrix[n].R].L = n;
    }

    int anchorY = main_matrix[n].anchorY;

    //se il primo della colonna è sotto di me
    if(cons_arr[anchorY].column_start == main_matrix[n].B){
        cons_arr[anchorY].column_start = n;
        //se non sono l'unico elemento
        if(main_matrix[n].B!=-1)
            main_matrix[main_matrix[n].B].T = n;
    }
    //se l'elemento da inserire diventa ultimo
    else if(cons_arr[anchorY].column_last_element == main_matrix[n].T){
        cons_arr[anchorY].column_last_element = n;
        main_matrix[main_matrix[n].T].B = n;
    }
    else{
        main_matrix[main_matrix[n].T].B = n;
        main_matrix[main_matrix[n].B].T = n;
    }
}


//rimuove un insieme di elementi che fanno parte della stessa riga
void remove_row_from_elmt(matrix_value *main_matrix, possible_value *poss_val_arr,constraint *cons_arr, int elmt, step *new_step){
    //vado a inizio riga
    int current_elmt = poss_val_arr[main_matrix[elmt].anchorX].row_start;
    while (current_elmt!=-1){
        remove_element(main_matrix,current_elmt,poss_val_arr,cons_arr);
        insertArray(&new_step->elements,current_elmt);
        current_elmt = main_matrix[current_elmt].R;
    } 
}


//rimuove tutte le colonne che hanno almeno un elemento della riga in input
void remove_columns_from_row(matrix_value *main_matrix, int row, possible_value *poss_val_arr,
 constraint *cons_arr, int *row_size, int *column_size, int *root, step *all_steps, int current_step, bool step_already_started){
    int current_column = poss_val_arr[row].row_start;
    if(!step_already_started){
        //LA GRANDEZZA DI UN ARRAY DI COLONNE PARTE DA 4, POI CRESCE SE HO PIÙ SCELTE DELLE X (CONTROLLARE QUANTO CRESCE)
        all_steps[current_step].isSplit = false;
        all_steps[current_step].currentX = -1;
    }
    initArray(&all_steps[current_step].columns,4); 
    initArray(&all_steps[current_step].elements,10);

    while (current_column!=-1){
        //elimino la colonna c'
        remove_column(cons_arr, main_matrix[current_column].anchorY,column_size,root);
        insertArray(&all_steps[current_step].columns,main_matrix[current_column].anchorY);
        int current_row = cons_arr[main_matrix[current_column].anchorY].column_start;
        while (current_row!=-1){
            //elimino uno per uno gli elementi dell'i-esima riga di c'
            remove_row_from_elmt(main_matrix,poss_val_arr,cons_arr,current_row,&all_steps[current_step]);
            current_row = main_matrix[current_row].B;
        }
        current_column = main_matrix[current_column].R;
    }
}
