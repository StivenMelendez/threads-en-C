#ifndef LIBRARY_H
#define LIBRARY_H

// Esta librería contiene funciones utiles para manejo de strings, matrices, lectura de archivos etc.
// No olviden modificarla a su estilo de programación.

//Sección de lectura de archivos 

//Leer un vector de un archivo

#include <stdio.h>
#include <stdlib.h>

int* read_vector_from_file(const char* fname, int *size){

    /*
        Solo funciona para archivos de la forma 
        n
        a_1
        a_2
        a_3
        ...
        a_n
        Donde n es el tamaño del vector y a_i son los elementos del vector.
    */

    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", fname);
        return NULL;
    }

    int size_temp;
    fscanf(fp, "%d", &size_temp);
    int* vector = (int*)malloc(size_temp * sizeof(int));
    if (vector == NULL) {
        printf("Error: No se pudo asignar memoria para el vector\n");
        fclose(fp);
        return NULL;
    }
    for (int i = 0; i < size_temp; i++) {
        fscanf(fp, "%d", &vector[i]);
    }
    fclose(fp);
    *size = size_temp;
    return vector;
}

//Leer una matriz de un archivo
//int** read_matrix_from_file(const char* fname, int *data1, int *data2, int *rows, int *cols){
int** read_matrix_from_file(const char* fname, int *rows, int *cols){

    /*
        Solo funciona para archivos de la forma 
        n m
        a_11 a_12 ... a_1m
        a_21 a_22 ... a_2m
        ...
        a_n1 a_n2 ... a_nm
        Donde n es el número de filas y m el número de columnas y a_ij son los elementos de la matriz.
    */

    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", fname);
        return NULL;
    }

    int rows_temp, cols_temp;
    fscanf(fp, "%d %d", &rows_temp, &cols_temp);
    //fscanf(fp, "%d %d %d %d", &data1, &data2, &rows_temp, &cols_temp);
    int** matrix = (int**)malloc(rows_temp * sizeof(int*));
    for (int i = 0; i < rows_temp; i++) {
        matrix[i] = (int*)malloc(cols_temp * sizeof(int));
    }
    for (int i = 0; i < rows_temp; i++) {
        for (int j = 0; j < cols_temp; j++) {
            fscanf(fp, "%d", &matrix[i][j]);
        }
    }
    fclose(fp);
    *rows = rows_temp;
    *cols = cols_temp;
    return matrix;
}

//Funciones de ventana para matrices

//Funcion para visitar una ventana nxm en una matriz de tamaño filas x columnas
// Esta función recorre una ventana de tamaño alto_ventana x ancho_ventana alrededor de cada elemento de la matriz
// Si la ventana debe ser cuadrada, entonces alto_ventana = ancho_ventana; por ejemplo alto_ventana = 3 y ancho_ventana = 3

void sumar_ventanas(int** matriz, int filas, int columnas, int alto_ventana, int ancho_ventana) {
    int mitad_alto = alto_ventana / 2;
    int mitad_ancho = ancho_ventana / 2;

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {

            int suma = 0;

            // Recorrer la ventana alrededor de (i, j)
            for (int di = -mitad_alto; di <= mitad_alto; di++) {
                for (int dj = -mitad_ancho; dj <= mitad_ancho; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    // Verificar que esté dentro de la matriz
                    if (ni >= 0 && ni < filas && nj >= 0 && nj < columnas) {
                        //Realizar la operación requerida, en este caso una suma
                        suma += matriz[ni][nj];
                    }
                }
            }

            printf("Suma en (%d,%d): %d\n", i, j, suma);
        }
    }
}

//Recorrer la diagolan principal de una matriz
void recorrer_diagonal(int** matriz, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        printf("(%d,%d): %d\n", i, i, matriz[i][i]);
    }
}

//Recorrer la triangular superior de una matriz
void recorrer_triangular_superior(int** matriz, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        for (int j = i; j < columnas; j++) {
            printf("(%d,%d): %d\n", i, j, matriz[i][j]);
        }
    }
}

//Recorrer la triangular inferior de una matriz
void recorrer_triangular_inferior(int** matriz, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j <= i && j < columnas; j++) {
            printf("(%d,%d): %d\n", i, j, matriz[i][j]);
        }
    }
}

//Recorrer la matriz en espiral
void recorrer_matriz_espiral(int** matriz, int filas, int columnas) {
    int top = 0, bottom = filas - 1, left = 0, right = columnas - 1;

    while (top <= bottom && left <= right) {
        for (int i = left; i <= right; i++) {
            printf("(%d,%d): %d\n", top, i, matriz[top][i]);
        }
        top++;

        for (int i = top; i <= bottom; i++) {
            printf("(%d,%d): %d\n", i, right, matriz[i][right]);
        }
        right--;

        if (top <= bottom) {

            for (int i = right; i >= left; i--) {
                printf("(%d,%d): %d\n", bottom, i, matriz[bottom][i]);
            }
            bottom--;
        }

        if (left <= right) {

            for (int i = bottom; i >= top; i--) {
                printf("(%d,%d): %d\n", i, left, matriz[i][left]);
            }
            left++;
        }
    }
}

/*
    Sección de Strings
*/

#include<string.h>


//Leer un archivo completo de strings usando fseek
char* read_file(const char* fname, int* file_size){
    //printf("Leyendo el archivo %s\n", fname);
    FILE* f = fopen(fname, "r");
    if(f == NULL){
        perror("File not found\n");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    char* content = (char*)malloc(size + 1);
    if(!content){
        perror("Memory error\n");
        return NULL;
    }

    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    *file_size = size;
    //printf("%s", content);
    return content;
}

//Leer un archivo linea por linea

char* read_file_line_to_line(const char* fname, int* file_size){
    FILE* f = fopen(fname, "r");
    if(!f){
        perror("File not found\n");
        return NULL;
    }

    int max_size = 1024;
    char buffer[max_size];
    int length = 0;

    char* content = (char*)malloc(max_size);
    if(!content){
        perror("Memory error\n");
        return NULL;
    }

    content[0] = '\0';

    while(fgets(buffer, sizeof(buffer), f)){

        size_t len = strlen(buffer);

        // Expandir si es necesario
        if (length + len + 1 > max_size) {
            max_size *= 2;
            char *temp = (char*)realloc(content, max_size);
            if (!temp) {
                perror("Fallo al ampliar memoria");
                free(content);
                fclose(f);
                return NULL;
            }
            content = temp;
        }

        strcpy(content + length, buffer);
        length += len;
    }

    fclose(f);
    return content;
}

//Esta funcion busca todas las ocurrencias en una cadena, pueden ser caracteres o sub cadenas. 
//resultados es un puntero a entero (vector dinamico) que guarda los indices donde se encuentra una ocurrencia
//debe ser pasado por referencia, y cantidad es un entero en el que se guarda la cantidad de ocurrencias
// que a su vez es el tamaño del vector anterior
/* Ejemplo:
    const char *texto = "abc hola abc mundo abc hola";
    const char *patron = "abc";

    int *posiciones = NULL;
    int cantidad = 0;

    buscar_ocurrencias(texto, patron, &posiciones, &cantidad);

*/
void buscar_ocurrencias(const char *texto, const char *patron, int **resultados, int *cantidad) {
    size_t len_texto = strlen(texto);
    size_t len_patron = strlen(patron);

    if (len_patron == 0 || len_patron > len_texto) {
        *resultados = NULL;
        *cantidad = 0;
        return;
    }

    int capacidad = 10;
    int *posiciones = (int*)malloc(capacidad * sizeof(int));
    if (!posiciones) {
        perror("Error al asignar memoria");
        *resultados = NULL;
        *cantidad = 0;
        return;
    }

    int cuenta = 0;
    for (int i = 0; i <= (int)(len_texto - len_patron); i++) {
        if (strncmp(&texto[i], patron, len_patron) == 0) {
            if (cuenta >= capacidad) {
                capacidad *= 2;
                int *temp = (int*)realloc(posiciones, capacidad * sizeof(int));
                if (!temp) {
                    perror("Error al redimensionar memoria");
                    free(posiciones);
                    *resultados = NULL;
                    *cantidad = 0;
                    return;
                }
                posiciones = temp;
            }
            posiciones[cuenta++] = i;
        }
    }

    *resultados = posiciones;
    *cantidad = cuenta;
}

//Invertir una cadena de caracteres
//Basta con pasar la cadena como parametros
//Recuerden que en C los punteros se pasan por referencia por defecto
/*
    si se pasa de esta forma
    reverse_str("Hola mundo");
    no va a funcionar. 
    la variale donde se almacena debe ser de tipo
    char[] o char*
*/
void reverse_str(char *str) {
    int inicio = 0;
    int fin = strlen(str) - 1;

    while (inicio < fin) {
        char temp = str[inicio];
        str[inicio] = str[fin];
        str[fin] = temp;
        inicio++;
        fin--;
    }
}


// Esta función elimina todas las ocurrencias de una subcadena en una cadena dada
// Modifica la cadena original in-place
// Parámetros:
//   str: cadena original a modificar
//   to_delete: subcadena a eliminar
// Retorno:
//   Número de ocurrencias eliminadas
int borrar_cadena(char *str, const char *to_delete) {
    if (str == NULL || to_delete == NULL || strlen(to_delete) == 0) {
        return 0;  // No hay nada que eliminar
    }

    int deleted_count = 0;
    char *pos;
    size_t len_to_delete = strlen(to_delete);

    // Buscar y eliminar todas las ocurrencias
    while ((pos = strstr(str, to_delete)) != NULL) {
        // Mover el resto de la cadena para sobreescribir la subcadena a eliminar
        memmove(pos, pos + len_to_delete, strlen(pos + len_to_delete) + 1);
        deleted_count++;
    }

    return deleted_count;
}
#endif