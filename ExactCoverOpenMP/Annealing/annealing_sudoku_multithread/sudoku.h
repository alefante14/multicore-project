#define SUDOKU_SIZE 9
#define SUDOKU_SQUARE 3
#define ANNEAL_TEMP_SAMPLE 10

typedef struct swappable{
    int value[SUDOKU_SIZE*2];
    int size;
}Swappable;

void populate_sudoku(int psudoku[SUDOKU_SIZE][SUDOKU_SIZE],Swappable sudoku_swappable[SUDOKU_SIZE])
{
    int available_numbers[SUDOKU_SIZE][SUDOKU_SIZE] = {1};
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            available_numbers[i][j] = 1;
        }
    }

    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            int calc = j / SUDOKU_SQUARE + (i /SUDOKU_SQUARE) * SUDOKU_SQUARE;
            if (psudoku
                    [i][j] != 0)
            {
                //printf("quad %d \n", (j / 3 + (i / 3) * 3));
                
                available_numbers[calc][psudoku[i][j] - 1] = 0;
            }
            else{
                sudoku_swappable[calc].value[sudoku_swappable[calc].size]=i;
                sudoku_swappable[calc].value[sudoku_swappable[calc].size+1]=j;
                //printf("calc: %d\n ",calc);
                //printf("sux %d \n",sudoku_swappable[calc].value[sudoku_swappable[calc].size]);
                //printf("suy %d \n",sudoku_swappable[calc].value[sudoku_swappable[calc].size+1]);
                sudoku_swappable[calc].size+=2;
            }
        }
    }
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            if (psudoku
                    [i][j] == 0)
            {
                //printf("quad %d \n", (j / 3 + (i / 3) * 3));
                int found = 0;
                int counter = 0;
                while (counter < SUDOKU_SIZE && found == 0)
                {
                    if (available_numbers[j / SUDOKU_SQUARE + (i / SUDOKU_SQUARE) * SUDOKU_SQUARE][counter] == 1)
                    {
                        found = counter + 1;
                        //printf("found %d\n", found);
                        available_numbers[j / SUDOKU_SQUARE + (i / SUDOKU_SQUARE) * SUDOKU_SQUARE][counter] = 0;
                    }
                    counter++;
                }
                psudoku[i][j] = found;
                //printf("%d\n", psudoku[i][j]);
                //available_numbers[j / 3 + (i / 3) * 3][psudoku[i][j]] = 1;
            }
        }
    }
    for(int i = 0; i<SUDOKU_SIZE;i++){
        for (int j = 0; j<=SUDOKU_SIZE;j++){
            //printf("BLOCCO %d VALORE %d :%d\n",i,j,sudoku_swappable[i].value[j]);
        }
    }
};


void print_sudoku(int psudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            if(psudoku[i][j] <=SUDOKU_SIZE){
                printf("0%d.", psudoku[i][j]);
            }
            else{
                printf("%d.", psudoku[i][j]);
            }
            
    }
    printf("\n");
    }
}

void swap_sudoku(int xa, int ya, int xb, int yb, int psudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
    int temp = psudoku[xa][ya];
    psudoku[xa][ya] = psudoku[xb][yb];
    psudoku[xb][yb] = temp;
    //printf("[%d,%d][%d,%d]\n", xa, ya, xb, yb);
}

void random_positions(int *xa, int *ya, int *xb, int *yb,Swappable sudoku_swappable[SUDOKU_SIZE])
{

    

    int chosen = rand() % SUDOKU_SIZE;
    //printf(freesudoku[quad_x*SUDOKU_SQUARE+quad_y]);
    //printf("\n");

    int randx = rand() % ((sudoku_swappable[chosen].size)/2);
    int randy = rand() % ((sudoku_swappable[chosen].size)/2);
    while(randy == randx && sudoku_swappable[chosen].size >2){
        randy = rand() % (((sudoku_swappable[chosen].size)/2)-1);
    }
    *xa = sudoku_swappable[chosen].value[2*randx];
    *ya = sudoku_swappable[chosen].value[2*randx+1];
    *xb = sudoku_swappable[chosen].value[2*randy];
    *yb = sudoku_swappable[chosen].value[2*randy+1];
    //printf("[%d]> swappable.size:%d QX:%d QY:%d Square %d |A: (%d %d) |B: (%d %d) -- randx:%d randy:%d\n",gettid(),sudoku_swappable[chosen].size,quad_x,quad_y,chosen,*xa,*ya,*xb,*yb,randx,randy);
    //printf(">>[%d][%d] - [%d][%d]\n", *xa, *ya, *xb, *yb);
    
    /*
    if (!(*xa - *xb > -SUDOKU_SQUARE && *xa - *xb < SUDOKU_SQUARE))
    {
        printf("[!]> Sussy imposter on X\n");
    }
    if (!(*ya - *yb > -SUDOKU_SQUARE && *ya - *yb < SUDOKU_SQUARE))
    {
        printf("[!]> Sussy imposter on Y\n");
    }
    */
        }



    
    
    //printf(">>[%d][%d] - [%d][%d]\n", *xa, *ya, *xb, *yb);

/*
void one_error(int psudoku[SUDOKU_SIZE][SUDOKU_SIZE],int pscore[SUDOKU_SIZE*2],int cscore){
    int temp_sudoku[SUDOKU_SIZE][SUDOKU_SIZE];
    int att_score[9];
    for(int i=0;i++;i<3){
        for(int j=0;j++;j<3){
            memcpy(temp_sudoku, psudoku, sizeof(int) * SUDOKU_SIZE * SUDOKU_SIZE);

        }   
     }
}
*/

int sudoku_from_file(FILE* puzzles, int(*sudoku)){
    //check if last
    fgetc(puzzles);
    if(feof(puzzles)){
    //printf("ooof");
        return -1;
        }
    fseek(puzzles,-1, SEEK_CUR);
    for(int x=0; x < SUDOKU_SIZE; x++){
        for(int y=0; y < SUDOKU_SIZE; y++){
            fscanf(puzzles, "%1d", &sudoku[x*SUDOKU_SIZE+y]);

        }
    }
    fseek(puzzles, 1, SEEK_CUR);
    return 0;
}
