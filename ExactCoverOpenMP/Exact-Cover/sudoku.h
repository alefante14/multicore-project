#define SUDOKU_SIZE 9

void print_sudoku(int psudoku[SUDOKU_SIZE][SUDOKU_SIZE]){
        for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            printf("%d ", psudoku[i][j]);
        }
        printf("\n");
    }
}

void print_sudoku_vector(int *psudoku)
{
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            printf("%d ", psudoku[i*SUDOKU_SIZE + j]);
        }
        printf("\n");
    }
}
int number_of_sudokus(FILE* puzzles){
    int ch;
    int lines = 0;
    while(!feof(puzzles)){
        ch = fgetc(puzzles);
        if(ch == '\n'){
            lines++;
        }
    }
    rewind(puzzles);
    return lines;
}
int sudoku_from_file(FILE* puzzles, int (*sudoku)){
    //check if last
    fgetc(puzzles);
    if(feof(puzzles))
        return -1;
    fseek(puzzles,-1, SEEK_CUR);
    for(int x=0; x < 9; x++){
        for(int y=0; y < 9; y++){
            fscanf(puzzles, "%1d", &sudoku[x*SUDOKU_SIZE + y]);
        }
    }
    fseek(puzzles, 1, SEEK_CUR);
    return 0;
}

int box_conv(int x, int y){
    int x_off;
    int y_off;
    if(x<3) x_off=0;
    else if(x<6) x_off=1;
    else x_off=2;
    if(y<3) y_off=0;
    else if(y<6) y_off=1;
    else y_off=2;
    return 3*y_off+x_off;
}