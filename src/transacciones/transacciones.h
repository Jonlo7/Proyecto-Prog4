#ifndef TRANSACCIONES_H
#define TRANSACCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "colors/colors.h"
#include "inventario/inventario.h"
#include <sqlite3.h>

#define MAX_FECHA 11 //YYYY-MM-DD

typedef enum {VENTA, COMPRA} tipoTransaccion;

typedef struct {
    Producto producto;          
    int cantidadTransaccion;     
    float totalItem;         
} ItemTransaccion;

typedef struct {
    ItemTransaccion *items;
    int numItems;
    int capacidadItems; 
    float totalTransaccion;
    char fecha[MAX_FECHA];
    tipoTransaccion tipo;
} Transaccion;

/**
 * Crea e inicializa una nueva transacción.
 * @return Puntero a la transacción creada o NULL en caso de error.
 */
Transaccion* crearTransaccion(void);

/**
 * Libera la memoria asignada a una transacción.
 * @param trans Puntero a la transacción a liberar.
 */
void liberarTransaccion(Transaccion* trans);

/**
 * Agrega un ítem a la transacción.
 * @param trans Puntero a la transacción.
 * @param prod Producto involucrado en el ítem de la transacción.
 * @param cantidad Cantidad del producto a agregar en la transacción.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int agregarItemTransaccion(Transaccion* trans, Producto prod, int cantidad);

/**
 * Calcula y actualiza el total de la transacción.
 * @param trans Puntero a la transacción.
 */
void calcularTotalTransaccion(Transaccion* trans);

/**
 * Guarda la transacción en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param trans Puntero a la transacción.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int guardarTransaccionDB(sqlite3* db, const Transaccion* trans);

/**
 * Menú interactivo para crear una transacción.
 * @param inv Puntero al inventario.
 * @param db Puntero a la base de datos.
 */
void menuCrearTransaccionDB(Inventario* inv, sqlite3* db);

#endif /* TRANSACCIONES_H */
