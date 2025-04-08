#include "huffman.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "arbol.h"
#include "pq.h"
#include "bitstream.h"
#include "confirm.h"

/*====================================================
     Constantes
  ====================================================*/

#define NUM_CHARS 256

/*
estructura para almacenar valores de un nodo de un arbol, 
c es el caracter
frec es la frecuencia
*/
typedef struct _keyvaluepair {
    char c;
    int frec;
} keyvaluepair;

/*====================================================
     Campo de bits.. agrega funciones si quieres
     para facilitar el procesamiento de bits.
  ====================================================*/

typedef struct _campobits {
    unsigned int bits;
    int tamano;
} campobits;

/**
    Esto utiliza aritmetica de bits para agregar un
   bit a un campo.
   
   Supongamos que bits->bits inicialmene es (en binario):
   
      000110001000
      
   y le quiero agregar un 1 en vez del segundo 0 (desde izq).
   Entonces, creo una "mascara" de la siguiente forma:
   
      1 << 11   me da 0100000000000

   Y entonces si juntamos los dos (utilizando OR binario):      
      000110001000
    | 0100000000000
    ----------------
      010110001000

    Esta funcion utiliza bits->tamano para decidir donde colocar
    el siguiente bit.
    
    Nota: asume que bits->bits esta inicialmente inicializado a 0,
    entonces agregar un 0, no requiere mas que incrementar bits->tamano.
*/      
static void bits_agregar(campobits* bits, int bit) {
    CONFIRM_RETURN(bits);
    CONFIRM_RETURN((unsigned int)bits->tamano < 8*sizeof(bits->bits));
    bits->tamano++;
    if (bit) {
        bits->bits = bits->bits | ( 0x1 << (bits->tamano-1));
    } 
}

/* ### University Template ###
    funcion de utilidad para leer un bit dentro de campobits dado el indice pos
    pos = 0, primer bit 
    pos = 1, segundo bit
    pos = 2, tercer bit
    pos = k, k bit

*/
static int bits_leer(campobits* bits, int pos) {
    CONFIRM_TRUE(bits,0);
    CONFIRM_TRUE(!(pos < 0 || pos > bits->tamano),0);
    // para saber si campobits tiene un 1 o 0 en la posicion dada 
    // recorro bits usando shift << y >>
    int bit = (bits->bits & (0x1 << (pos))) >> (pos);
    return bit;
}

/** Agus
 * Prints the campobits value to the screen.
 *
 * This function iterates from the least significant bit (at index 0
 * up to the most significant (index tamano-1) using the bits_leer() helper, and
 * prints each bit as '0' or '1'.
 *
 * @param bits A pointer to a campobits structure.
 */
 static void bits_print(campobits* bits) {
    int i;

    if (!bits) {
        printf("NULL\n");
        return;
    }

    for (i = 0; i < bits->tamano; i++) {
        int bit = bits_leer(bits, i);  // Use bits_leer to read each bit
        putchar(bit ? '1' : '0');
    }

    putchar('\n');
}

static int bits_remove_last(campobits* bits) {
    if (!bits || bits->tamano <= 0){
        return -1; // Error: No bits to remove
    }

    // Retrieve the last bit at position (tamano - 1)
    int last_bit = (bits->bits >> (bits->tamano - 1)) & 0x1;
    
    // Decrease the size to reflect the removal of the last bit
    bits->tamano--;
    
    // Clear the bit at the new last position
    bits->bits &= ~(0x1 << bits->tamano);
    
    return last_bit;
}

static void testCampobitsBitstream() {
    BitStream bs = NULL;
    BitStream bsIn = NULL;
    char* testbitstreamtxt = "testbitsteam.txt";
    int i = 0;
    // crear un campobits
    campobits* b = (campobits*)malloc(sizeof(struct _campobits));
    CONFIRM_RETURN(b);
    b->bits = 0;
    b->tamano = 0;
    // ej quiero codificar 00101 
    bits_agregar(b, 0);
    bits_agregar(b, 0);
    bits_agregar(b, 1);
    bits_agregar(b, 0);
    bits_agregar(b, 1);
    // crear un archivo y escribir bit a bit
    bs = OpenBitStream(testbitstreamtxt, "w");
    // para escribir en un archivo PutBit agrega bits 
    // ej recorro el campobits y agrego bit a bit
    for (i = 0; i < b->tamano; i++) {
        int bit = bits_leer(b, i);
        PutBit(bs, bit);
    }
    // PutByte para escribir 1 byte completo, 
    // ej agrego un caracter
    PutByte(bs, 'z');
    // no olvidar cerrar el archvio
    CloseBitStream(bs);
    // y liberar memoria utilizada
    free(b);
    // Mi archivo entonces contiene: 00101z
    // si quiero leer el mismo
    bsIn = OpenBitStream(testbitstreamtxt, "r");
    // leer bit a bit
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    // leer un byte
    printf("%c\n", GetByte(bsIn));
    CloseBitStream(bsIn);



}

/*Agus*/
static void imprimirNodoEjemplo(Arbol nodo) {
    CONFIRM_RETURN(nodo);
    keyvaluepair* val = (keyvaluepair*)arbol_valor(nodo);
    printf("%d,%c\n", val->frec, val->c);
}

static void testArbol() {

    /* si quiero crear un arbol que contiene:
    char freq
    a    4
    s    2
    entonces tengo un nodo padre
    con freq 6 y dos hijos, con freq 2  y 4 con sus caracteres correspondientes
    */
    keyvaluepair* v1 = malloc(sizeof(struct _keyvaluepair));
    keyvaluepair* v2 = malloc(sizeof(struct _keyvaluepair));
    keyvaluepair* v3 = malloc(sizeof(struct _keyvaluepair));

    Arbol n1;
    Arbol n2;
    Arbol n3;

    CONFIRM_RETURN(v1);
    CONFIRM_RETURN(v2);
    CONFIRM_RETURN(v3);

    v1->c = 'a';
    v1->frec = 4;
    v2->c = 's';
    v2->frec = 2;
    v3->c = ' ';
    v3->frec = 6;

    n1 = arbol_crear(v1);
    n2 = arbol_crear(v2);
    n3 = arbol_crear(v3);

    arbol_agregarIzq(n3, n2);
    arbol_agregarDer(n3, n1);
    
    //ejemplo de recorrer el arbol e imprimir nodos con valor keyvaluepair
    arbol_imprimir(n3, imprimirNodoEjemplo);
    arbol_destruir(n3);
    free(v1);
    free(v2);
    free(v3);
}

void campobitsDemo() {
    printf("***************CAMPOBITS DEMO*******************\n");
    testCampobitsBitstream();
    printf("***************ARBOL DEMO******************\n");
    testArbol();
    printf("***************FIN DEMO*****************\n");
}
/*====================================================
     Declaraciones de funciones 
  ====================================================*/

/* Puedes cambiar esto si quieres.. pero entiende bien lo que haces */
static int calcular_frecuencias(int* frecuencias, char* entrada);
static Arbol crear_huffman(int* frecuencias);
static int codificar(Arbol T, char* entrada, char* salida);
static void crear_tabla(campobits* tabla, Arbol T, campobits *bits);

static Arbol leer_arbol(BitStream bs);
static void decodificar(BitStream in, BitStream out, Arbol arbol);

static void imprimirNodo(Arbol nodo);

/*====================================================
     Implementacion de funciones publicas
  ====================================================*/

/** Agus
 * Prints the frequency of ASCII characters stored in an array.
 *
 * @param freq Pointer to an integer array of size 256 where each index represents an ASCII value
 *             and the value at that index is the frequency of that character.
 */
 void print_frequency(const int* freq) {
    int i;

    if (!freq) {
        fprintf(stderr, "Error: Null frequency array.\n");
        return;
    }

    for (i = 0; i < NUM_CHARS; i++) {
        if (freq[i] > 0) {
            if (i >= 32 && i < 127)
                printf("Character '%c' (ASCII %d): %d times\n", i, i, freq[i]);
            else
                printf("ASCII %d: %d times\n", i, freq[i]);
        }
    }
}

/*
  Comprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int comprimir(char* entrada, char* salida) {
    
    /* 256 es el numero de caracteres ASCII.
       Asi podemos utilizar un unsigned char como indice.
     */
    int frecuencias[NUM_CHARS]; 
    Arbol arbol = NULL;

    /* Primer recorrido - calcular frecuencias */
    CONFIRM_TRUE(0 == calcular_frecuencias(frecuencias, entrada), 0);
            
    arbol = crear_huffman(frecuencias);
    printf("############ TREE ############\n");
    arbol_imprimir(arbol, imprimirNodo); 
    printf("########## END TREE ##########\n");

    /* Segundo recorrido - Codificar archivo */
    CONFIRM_TRUE(0 == codificar(arbol, entrada, salida), 0);
    
    arbol_destruir(arbol);
    
    return 0;
}


/*
  Descomprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int descomprimir(char* entrada, char* salida) {

    BitStream in = 0;
    BitStream out = 0;
    Arbol arbol = NULL;
        
    /* Abrir archivo de entrada */
    in = OpenBitStream(entrada, "r");
    
    /* Leer Arbol de Huffman */
    arbol = leer_arbol(in);
    arbol_imprimir(arbol, imprimirNodo);

    /* Abrir archivo de salida */
    out = OpenBitStream(salida, "w");
    
    /* Decodificar archivo */
    decodificar(in, out, arbol);
    
    CloseBitStream(in);
    CloseBitStream(out);
    return 0;
}

/*====================================================
     Funciones privadas
  ====================================================*/


/** Agus
 *  Helper funtion to check if a node is a leaf or not.
 *  
 * @param tree A node of a Tree.
 * @return int (boolean like).
 */
 int IsLeaf(Arbol tree){
    return (NULL == arbol_izq(tree) && NULL == arbol_der(tree));
}

/** Agus
 * Reads a file character by character and counts the frequency of each ASCII character,
 * storing the result in the provided array.
 *
 * @param frecuencias Pointer to an integer array of size 256 where each index represents an ASCII value
 *                    and the value at that index is the frequency of that character.
 * @param entrada The name (or path) of the file to be read.
 * @return 0 if successful, non-zero if an error occurs.
 */
static int calcular_frecuencias(int* frecuencias, char* entrada) {
    if (!frecuencias || !entrada) {
        fprintf(stderr, "Error: Null pointer argument.\n");
        return -1;
    }

    // Initialize all frequency counts to zero
    for (int i = 0; i < NUM_CHARS; i++) {
        frecuencias[i] = 0;
    }

    // Open the file in read mode
    FILE* file = fopen(entrada, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Read the file character by character and count occurrences
    int character;
    while ((character = fgetc(file)) != EOF) {
        if (character >= 0 && character < NUM_CHARS) {
            frecuencias[character]++;
        }
    }

    //print_frequency(frecuencias); // Just for debuging

    // Close the file
    fclose(file);

    return 0;
}



/** Agus
* Build a Huffman Tree with the given frequencies. 
* 
* @param frecuencias An Array of int, the index represents an ASCII and the value the frequency of each.
* return Arbol A complete huffman tree.
*/
static Arbol crear_huffman(int* frecuencias) {
    int i;
    PQ pq = pq_create();
    if (!pq) {
        /* Memory allocation error for priority queue */
        return NULL;
    }
    
    /* Insert each character with its frequency into the priority queue */
    for (i = 0; i < 256; i++) {
        if (frecuencias[i] > 0) {
            keyvaluepair* kv = (keyvaluepair*) malloc(sizeof(keyvaluepair));
            if (!kv) {
                /* Handle memory allocation failure */
                pq_destroy(pq);
                return NULL;
            }
            kv->c = (char)i;
            kv->frec = frecuencias[i];
            
            Arbol node = arbol_crear((void*)kv);
            if (!node) {
                free(kv);
                pq_destroy(pq);
                return NULL;
            }
            if (!pq_add(pq, node, kv->frec)) {
                arbol_destruir(node);
                pq_destroy(pq);
                return NULL;
            }
        }
    }
    
    /* If no characters were inserted, free PQ and return NULL */
    if (pq_size(pq) == 0) {
        pq_destroy(pq);
        return NULL;
    }
    
    /* Construct Huffman tree */
    while (pq_size(pq) > 1) {
        Arbol left, right;
        /* Remove the tree with minimum frequency */
        if (!pq_remove(pq, (void**)&left)) {
            pq_destroy(pq);
            return NULL;
        }
        if (!pq_remove(pq, (void**)&right)) {
            pq_destroy(pq);
            return NULL;
        }
        
        /* Allocate memory for the parent node's keyvaluepair */
        keyvaluepair* parent_kv = (keyvaluepair*) malloc(sizeof(keyvaluepair));
        if (!parent_kv) {
            pq_destroy(pq);
            return NULL;
        }
        parent_kv->c = '\0'; /* Internal node, no character */
        parent_kv->frec = ((keyvaluepair*) arbol_valor(left))->frec +
                          ((keyvaluepair*) arbol_valor(right))->frec;
        
        /* Create parent tree node and assign children */
        Arbol parent = arbol_crear((void*)parent_kv);
        if (!parent) {
            free(parent_kv);
            pq_destroy(pq);
            return NULL;
        }
        arbol_agregarIzq(parent, left);
        arbol_agregarDer(parent, right);
        
        /* Insert the new tree into the priority queue */
        if (!pq_add(pq, parent, parent_kv->frec)) {
            arbol_destruir(parent);
            pq_destroy(pq);
            return NULL;
        }
    }
    
    /* Get the root of the Huffman tree */
    Arbol huffman_tree;
    if (!pq_remove(pq, (void**)&huffman_tree)) {
        pq_destroy(pq);
        return NULL;
    }
    
    /* Destroy the priority queue; the final tree is not freed by pq_destroy */
    pq_destroy(pq);
    
    return huffman_tree;
}

/** Agus
* Helper function to create the Huffman coded table.
* It traverses the Huffman tree from the root to each leaf,
* accumulating the path in a campobits structure:
*   - Append 0 when moving to the left.
*   - Append 1 when moving to the right.
* For each leaf encountered, it stores the accumulated campobits
* in the table using the ASCII value of the leaf's character as the index.
*/
static void create_huffman_table(Arbol T, campobits current_code, campobits table[]) {
    /* If the tree is empty, just return */
    if (T == NULL) {
        return;
    }

    // Check if T is a leaf (no left and no right child)
    if (arbol_izq(T) == NULL && arbol_der(T) == NULL) {
        keyvaluepair* kv = (keyvaluepair*) arbol_valor(T);
        table[(unsigned char) kv->c] = current_code;  // Save the accumulated code for this character

        printf("%c: ", kv->c); // Just Debugging
        bits_print(&current_code); // Just Debugging

        return;
    }

    {
        /* Traverse left: append 0 */
        campobits left_code = current_code;
        bits_agregar(&left_code, 0);
        create_huffman_table(arbol_izq(T), left_code, table);
    }

    {
        /* Traverse right: append 1 */
        campobits right_code = current_code;
        bits_agregar(&right_code, 1);
        create_huffman_table(arbol_der(T), right_code, table);
    }
}

/* Agus
   Recursive helper function to traverse the Huffman tree in pre-order and write its structure to a BitStream.
   
   - Internal nodes are represented by writing a '0' to the output.
   - Leaf nodes are represented by writing a '1' followed by the ASCII byte of the character.
   
   This function does not use an accumulating campobit, as it directly writes to the BitStream 
   while traversing the tree.
*/
static void write_tree_preorder(Arbol T, BitStream out) {
    if (T == NULL) {
        return;
    }

    // If T is a leaf, save 1 in the out file and then the char's ASCII byte
    if (arbol_izq(T) == NULL && arbol_der(T) == NULL) {

        keyvaluepair* kv = (keyvaluepair*) arbol_valor(T);

        PutBit(out, 1);
        PutByte(out, kv->c);

        //printf("1%c", kv->c); // Just debugging

        return;
    } else {
        PutBit(out, 0);
        //printf("0"); // Just debugging
        
        write_tree_preorder(arbol_izq(T), out);
        write_tree_preorder(arbol_der(T), out);
    }
}

/* Agus
   Encodes the input file using the Huffman tree and writes the result to the output file.
   
   Parameters:
     T       - The Huffman tree.
     entrada - Name of the input file to encode.
     salida  - Name of the output file where the encoded data will be written.
   
   Returns:
     0 on success, or a nonzero value if an error occurs.
   
   The function performs the following:
     1. Creates an array (codes_table_campobits) of campobits of size NUM_CHARS,
        where each index corresponds to an ASCII value and contains its Huffman code.
     2. Opens the input and output files.
     3. // ### Tree Code Building and Writing ###
        Traverses the Huffman tree in pre-order, accumulating the path in a campobit.
        When a leaf is found, it writes the accumulated campobit followed by the character 
        (by calling write_tree_code) to the output BitStream, then resets the campobit.
     4. Calls create_huffman_table to fill codes_table_campobits.
     5. Reads the input file character by character and for each character, writes its 
        corresponding campobit from codes_table_campobits to the output file.
     6. Closes files and cleans up resources.
*/
static int codificar(Arbol T, char* entrada, char* salida) {
    FILE* in = NULL;
    BitStream out = NULL;
    int i;
    int c;
    
    // Create the table for Huffman codes. Each entry corresponds to a character (0..NUM_CHARS-1).
    campobits codes_table_campobits[NUM_CHARS];
    memset(codes_table_campobits, 0, NUM_CHARS * sizeof(campobits));
    
    // Open the input file for reading
    in = fopen(entrada, "r");
    if (in == NULL) {
        return -1;
    }
    
    // Open the output file as a BitStream for writing
    out = OpenBitStream(salida, "w");
    if (out == NULL) {
        fclose(in);
        return -1;
    }
    
    // ### Tree Code Building and Writing ###
    write_tree_preorder(T, out);
    // END of Section

    // Build the Huffman code table by traversing the tree
    create_huffman_table(T, (campobits){0, 0}, codes_table_campobits);
    
    /* Write the encoded text.
    For each character in the input file, write its corresponding
    bit sequence (campobit) stored in the table. */
    fseek(in, 0, SEEK_SET);
    while ((c = fgetc(in)) != EOF) {
        if (c < 0 || c >= NUM_CHARS) {
            continue;
        }
        
        campobits code = codes_table_campobits[(unsigned char)c];
        for (i = 0; i < code.tamano; i++) {
            int bit = bits_leer(&code, i);
            PutBit(out, bit);
        }
    }
    
    // Clean up: close input file and BitStream output
    fclose(in);
    CloseBitStream(out);
    
    return 0;
}

/** Agus
 * Creates a new Huffman tree node with a keyvaluepair containing the given value.
 *
 * The function allocates a keyvaluepair structure, assigns the provided character
 * (which can be a valid char or '\0') to the 'c' field, sets the frequency ('frec')
 * to 0, and then creates a new tree node (of type Arbol) containing this keyvaluepair.
 * 
 * Going to be honest, understanding how an Arbol really works was a pain until I got it.
 *
 * @param value The character value to be stored in the node's keyvaluepair.
 * @return A new Arbol node containing the keyvaluepair, or exits on memory allocation error.
 */
 Arbol create_node(char value) {
    keyvaluepair* kv = (keyvaluepair*) malloc(sizeof(keyvaluepair));
    if (kv == NULL) {
        fprintf(stderr, "Memory allocation error in create_node (keyvaluepair allocation)\n");
        exit(1);
    }
    kv->c = value;    // Set the value (can be a valid char or '\0')
    kv->frec = 0;     // Initialize frequency to 0

    Arbol node = arbol_crear((void*)kv);
    if (node == NULL) {
        fprintf(stderr, "Memory allocation error in create_node (arbol_crear failed)\n");
        free(kv);
        exit(1);
    }
    return node;
}

/** Agus
*  Helper function to read a bit from the input file.
*  check if it's a leaf (1), then get a byte, create a node and stores the byte in it.
*  else, it's not a leaf, creates two empty nodes and make them the current node children.
*/
void decode_tree(Arbol node, void* data){
    if (NULL == node) return;

    BitStream bs = *(BitStream*)data;
    int bit = GetBit(bs);

    
    if (bit == 1){
        int byte = GetByte(bs);
        keyvaluepair* kv = (keyvaluepair*) arbol_valor(node);
        kv->c = (char)byte;
        //printf("1%c", kv->c); // Just for debugging
        return;
    } else {
        Arbol left = create_node('\0');
        Arbol right = create_node('\0');

        if (!left || !right) return;

        arbol_agregarIzq(node, left);
        arbol_agregarDer(node, right);
        
        //printf("0"); // Just for debugging
        return;
    }
}

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Para leer algo que esta guardado en preorden, hay que
   pensarlo un poquito.
   
   Pero basicamente la idea es que vamos a leer el archivo
   en secuencia. Inicialmente, el archivo probablemente va 
   a empezar con el bit 0 representando la raiz del arbol. 
   Luego, tenemos que leer recursivamente (utiliza otra funcion
   para ayudarte si lo necesitas) un nodo izquierdo y uno derecho.
   Si uno (o ambos) son hojas entonces tenemos que leer tambien su
   codigo ASCII. Hacemos esto hasta que todos los nodos tienen sus 
   hijos. (Si esta bien escrito el arbol el algoritmo terminara
   porque no hay mas nodos sin hijos)
*/
static Arbol leer_arbol(BitStream bs) {
    if (!bs) return NULL; // Entry Verification

    //int bit = GetBit(bs);

    // If it's a 0, it's an internal node
    Arbol root = create_node('\0');
    if (NULL == root) return NULL; // Memory allocation failure

    arbol_preorden(root, decode_tree, &bs);

    return root;
}

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Ahora lee todos los bits que quedan en in, y escribelos como bytes
   en out. Utiliza los bits para navegar por el arbol de huffman, y
   cuando llegues a una hoja escribe el codigo ASCII al out con PutByte()
   y vuelve a comenzar a procesar bits desde la raiz.
   
   Sigue con este proceso hasta que no hay mas bits en in.
*/   
static void decodificar(BitStream in, BitStream out, Arbol arbol) {
    
    if (!in || !out || !arbol) return; // Entry verification

    while (!IsEmptyBitStream(in)) {
        Arbol current = arbol; // Comenzar desde la raíz en cada símbolo
        
        // Recorrer el árbol bit a bit hasta encontrar una hoja
        while (!IsLeaf(current)) {
            int bit = GetBit(in);
            if (bit == 0) {
                current = arbol_izq(current);
            } else {
                current = arbol_der(current);
            }
        }
        
        // Escribimos el byte correspondiente al carácter de la hoja
        keyvaluepair* kv = (keyvaluepair*) arbol_valor(current);
        PutByte(out, kv->c);
    }

}


/* Esto es para imprimir nodos..
   Tal vez tengas mas de uno de estas funciones debendiendo
   de como decidiste representar los valores del arbol durante
   la compresion y descompresion.
*/
static void imprimirNodo(Arbol nodo) {
    // Check that the node is not NULL
    CONFIRM_RETURN(nodo);
    
    // Retrieve the key-value pair stored in the node
    keyvaluepair* val = (keyvaluepair*) arbol_valor(nodo);
    
    // Check if the node is a leaf (no left or right child)
    if (arbol_izq(nodo) == NULL && arbol_der(nodo) == NULL) {
        // Print leaf node information: frequency and character
        printf("Leaf: %d, %c", val->frec, val->c);
    } else {
        // For internal nodes, we might not have a meaningful character, so we print only the frequency
        printf("Internal: %d", val->frec);
    }
}