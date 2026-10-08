#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIO 10

typedef struct HashNode {
    char *clave;
    int valor;
    struct HashNode *siguiente;
} HashNode;

typedef struct {
    HashNode *tabla[TAMANIO];
} HashTable;

unsigned int funcion_hash(const char *clave) {
    unsigned int hash = 0;
    while (*clave) {
        hash = (hash * 31) + (unsigned char)*clave;
        clave++;
    }
    return hash % TAMANIO;
}

// Inicializar la tabla (todos los punteros en NULL)
void hash_init(HashTable *ht) {
    for (int i = 0; i < TAMANIO; i++) {
        ht->tabla[i] = NULL;
    }
}

// Insertar: si existe la misma clave con el MISMO valor, no duplica;
// si el valor es distinto, agrega un nodo nuevo a la lista.
// Devuelve 1 si insertó, 0 si ya existía el par, -1 si falló la memoria.
int hash_insertar(HashTable *ht, const char *clave, int valor) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0 && actual->valor == valor) {
            return 0; // el par clave-valor ya está
        }
        actual = actual->siguiente;
    }

    HashNode *nuevo = (HashNode *)malloc(sizeof(HashNode));
    if (nuevo == NULL) return -1;

    nuevo->clave = strdup(clave);
    if (nuevo->clave == NULL) {
        free(nuevo);
        return -1;
    }

    nuevo->valor = valor;
    nuevo->siguiente = ht->tabla[indice]; // inserta al inicio de la lista
    ht->tabla[indice] = nuevo;
    return 1;
}

// Buscar: devuelve el valor más reciente de esa clave
int hash_buscar(HashTable *ht, const char *clave, int *encontrado) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            *encontrado = 1;
            return actual->valor;
        }
        actual = actual->siguiente;
    }

    *encontrado = 0;
    return -1;
}

// Imprimir todos los valores asociados a una clave
void hash_buscar_todos(HashTable *ht, const char *clave) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    int hay = 0;

    printf("%s:", clave);
    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            printf(" %d", actual->valor);
            hay = 1;
        }
        actual = actual->siguiente;
    }
    if (!hay) printf(" (no encontrada)");
    printf("\n");
}

// Borrar TODAS las entradas con esa clave. Devuelve cuántas borró.
int hash_borrar(HashTable *ht, const char *clave) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    HashNode *anterior = NULL;
    int borrados = 0;

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            HashNode *a_borrar = actual;

            if (anterior == NULL) {
                ht->tabla[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            actual = actual->siguiente; // avanzar ANTES de liberar
            // 'anterior' no cambia: sigue apuntando al nodo previo válido

            free(a_borrar->clave);
            free(a_borrar);
            borrados++;
        } else {
            anterior = actual;
            actual = actual->siguiente;
        }
    }
    return borrados;
}

// Borrar solo un par clave-valor específico. Devuelve 1 si lo borró.
int hash_borrar_par(HashTable *ht, const char *clave, int valor) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    HashNode *anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0 && actual->valor == valor) {
            if (anterior == NULL) {
                ht->tabla[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            free(actual->clave);
            free(actual);
            return 1;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return 0;
}

// Liberar toda la memoria de la tabla
void hash_destruir(HashTable *ht) {
    for (int i = 0; i < TAMANIO; i++) {
        HashNode *actual = ht->tabla[i];
        while (actual != NULL) {
            HashNode *temporal = actual;
            actual = actual->siguiente;
            free(temporal->clave);
            free(temporal);
        }
        ht->tabla[i] = NULL;
    }
}

int main(void) {
    HashTable HT;
    hash_init(&HT);               // importante: inicializar

    hash_insertar(&HT, "Ernesto", 10);
    hash_insertar(&HT, "Braulio", 15);
    hash_insertar(&HT, "Catalina", 56);
    hash_insertar(&HT, "Alondra", 18);
    hash_insertar(&HT, "Freddy", 59);
    hash_insertar(&HT, "Ernesto", 18);  // misma clave, valor distinto -> nodo nuevo
    hash_insertar(&HT, "Ernesto", 18);  // mismo par -> se ignora

    hash_buscar_todos(&HT, "Ernesto");  // Ernesto: 18 10

    int encontrado;
    int v = hash_buscar(&HT, "Ernesto", &encontrado);
    printf("Valor mas reciente de Ernesto: %d (encontrado=%d)\n", v, encontrado);

    hash_borrar_par(&HT, "Ernesto", 18);
    hash_buscar_todos(&HT, "Ernesto");  // Ernesto: 10

    printf("Borrados de Braulio: %d\n", hash_borrar(&HT, "Braulio"));
    hash_buscar_todos(&HT, "Braulio");  // no encontrada

    hash_destruir(&HT);           // liberar toda la memoria
    return 0;
}