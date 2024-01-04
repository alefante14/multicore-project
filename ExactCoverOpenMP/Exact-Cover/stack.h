#include <stdlib.h>
#include <stdio.h>

/*struttura arraylist contiene un vettore di unsigned long che rappresentano il valore numerico della locazione in memoria dell'oggetto
oppure di contenere un valore numerico positivo (come il client fd)*/
typedef struct arraylist {
  int *array; 
  size_t used; 
  size_t size;
 } arraylist;

/*Inizializza l'arraylist con la lunghezza dell'array inziale passata in input,
la mutex non è inizializzata perché potrebbe non servire sempre, bisogna inizializzarla 
solo di volontà propria*/
void initArray(arraylist *a, size_t initialSize) {
  a->array = (int*) calloc(initialSize, sizeof(int)); //alloca il vettore
  a->used = 0; //setta a 0 l'utilizzo della lista
  a->size = initialSize; //inserisce il contatore all'inizial size della lista
}

/*inserisce elemento in coda alla lista*/
void insertArray(arraylist *a, int element) {
  if(a->used == a->size) { //se gli usati sono uguali agli utilizzati allora bisogna riallocare l'array
    a->size *= 2; //la nuova size è uguale al doppio della vecchia
    a->array = (int*) realloc(a->array, a->size * sizeof(int));//realloca l'array
  }
  a->array[a->used++] = element; //inserisce l'elemento all'untima posizione e la incrementa
}


/*libera l'array e reimposta le size*/
void freeArray(arraylist *a) {
  free(a->array); //libera il vettore
  a->array = NULL; //nullifica il vecchio puntatore al vettore
  a->used = a->size = 0; //setta a zero i vecchi contatori
}


int popArray(arraylist* a){
  if(a->used == 0){
    freeArray(a);
    return -1;
  }
  a->used--;
  return a->array[a->used];
}



