#include "pq.h"
#include <stdlib.h>

/* Función auxiliar para intercambiar dos elementos del arreglo */
static void swap(PrioValue* a, PrioValue* b) {
    PrioValue temp = *a;
    *a = *b;
    *b = temp;
}

/* Propaga el elemento en la posición i hacia arriba para mantener el min-heap */
static void percolate_up(PQ pq, int i) {
    while(i > 1) {
        int parent = i / 2;
        if (pq->arr[parent]->prio <= pq->arr[i]->prio)
            break;
        swap(&pq->arr[parent], &pq->arr[i]);
        i = parent;
    }
}

/* Propaga el elemento last hacia abajo a partir de la posición i para mantener el min-heap */
static void percolate_down(PQ pq, int i, PrioValue last) {
    int child;
    while (i * 2 <= pq->size) {
        child = i * 2;
        if (child < pq->size && pq->arr[child+1]->prio < pq->arr[child]->prio) {
            child++;
        }
        if (last->prio <= pq->arr[child]->prio)
            break;
        pq->arr[i] = pq->arr[child];
        i = child;
    }
    pq->arr[i] = last;
}

/* Crea la cola de prioridad PQ e inicializa sus atributos
retorna un puntero a la cola de prioridad 
retorna NULL si hubo error*/
PQ pq_create() {
   PQ pq = calloc(1, sizeof(struct Heap));
   if (!pq){
      return NULL;
   }
   pq->cap = 0;
   pq->size = 0;
   pq->arr = NULL;
   return pq;
}

/*
Agrega un valor a la cola con la prioridad dada

retorna TRUE si tuvo exito, FALSE si no
*/
BOOLEAN pq_add(PQ pq, void* valor, int prioridad) {
   if (!pq)
       return FALSE;
   
   /* Verifica si es necesario expandir el arreglo. 
      Como el heap se maneja en posiciones 1..size, se necesita cap >= size+1 */
   if (pq->size + 1 > pq->cap) {
       int newCap = (pq->cap == 0) ? 1 : pq->cap * 2;
       PrioValue* newArr = realloc(pq->arr, (newCap + 1) * sizeof(PrioValue));
       if (!newArr)
           return FALSE;
       pq->arr = newArr;
       pq->cap = newCap;
   }
   
   /* Crea un nuevo contenedor para el valor y su prioridad */
   PrioValue newElement = calloc(1, sizeof(struct _PrioValue));
   if (!newElement)
       return FALSE;
   
   newElement->prio = prioridad;
   newElement->value = valor;
   
   /* Inserta el nuevo elemento al final del heap y lo ajusta */
   pq->size++;
   int i = pq->size;
   pq->arr[i] = newElement;
   percolate_up(pq, i);
   
   return TRUE;
}

/* 
  Saca el valor de menor prioridad (cima del monticulo) y lo guarda en la posicion retVal (paso por referencia)
  retorna FALSE si tiene un error
  retorna TRUE si tuvo EXITO
*/
BOOLEAN pq_remove(PQ pq, void** retVal) {
   if (!pq || pq->size == 0 || retVal == NULL)
       return FALSE;
   
   /* Guarda el elemento de la raíz */
   PrioValue removed = pq->arr[1];
   *retVal = removed->value;
   
   /* Toma el último elemento y reestablece el heap */
   PrioValue last = pq->arr[pq->size];
   pq->size--;
   if (pq->size > 0) {
       percolate_down(pq, 1, last);
   }
   
   /* Libera el contenedor del elemento removido */
   free(removed);
   return TRUE;
}

/* retorna el tama�o de la cola de prioridad, 
   retorna 0 si hubo error 
 */
int pq_size(PQ pq) {
   if (!pq)
      return 0;
   return pq->size;
}

/* Destruye la cola de prioridad, 
retorna TRUE si tuvo exito
retorna FALSE si tuvo error*/
BOOLEAN pq_destroy(PQ pq) {
   if (!pq)
        return FALSE;
    
    for (int i = 1; i <= pq->size; i++) {
        free(pq->arr[i]);
    }
    free(pq->arr);
    free(pq);
    return TRUE;
}
