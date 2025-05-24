#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "colors/colors.h"
#include <sqlite3.h>

#define MAX_NOMBRE 50

typedef struct {
    int id;
    char nombre[MAX_NOMBRE+1];
    float precio;
    int stock;
    bool activo;
} Producto;

typedef struct {
    Producto* productos;   
    size_t cantidad;      
    size_t capacidad;     
} Inventario;

/**
 * Crea e inicializa un inventario en memoria.
 * @return Retorna un puntero al inventario creado o NULL en caso de error.
 */
Inventario* crearInventario(void);

/**
 * Libera la memoria asignada al inventario.
 * @param inv Puntero al inventario a liberar.
 */
void liberarInventario(Inventario* inv);

/**
 * Carga el inventario desde la base de datos SQLite.
 * @param inv Puntero al inventario a cargar.
 * @param db Puntero a la base de datos.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int cargarInventarioDB(Inventario* inv, sqlite3* db);

/**
 * Busca un producto por su ID en el inventario.
 * @param inv Puntero al inventario.
 * @param id ID del producto a buscar.
 * @return Retorna un puntero al producto encontrado o NULL si no se encuentra.
 */
Producto* buscarProducto(Inventario* inv, int id);

/**
 * Inserta un nuevo producto en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param nombre Nombre del producto.
 * @param precio Precio del producto.
 * @param stock Stock del producto.
 * @param activo Estado activo del producto (true o false).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int insertarProductoDB(sqlite3* db, const char* nombre, float precio, int stock, bool activo);

/**
 * Modifica los datos de un producto existente en la base de datos.
 * @param db Puntero a la base de datos.
 * @param id ID del producto a modificar.
 * @param nuevoNombre Nuevo nombre del producto (puede ser NULL para no modificarlo).
 * @param nuevoPrecio Nuevo precio del producto (puede ser -1 para no modificarlo).
 * @param nuevoStock Nuevo stock del producto (puede ser -1 para no modificarlo).
 * @param nuevoActivo Nuevo estado activo del producto.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int modificarProductoDB(sqlite3* db, int id, const char* nuevoNombre, float nuevoPrecio, int nuevoStock, bool nuevoActivo);

/**
 * Actualiza el stock de un producto en la base de datos.
 * @param db Puntero a la base de datos.
 * @param id ID del producto a actualizar.
 * @param cantidad Cantidad a agregar (puede ser negativa para restar).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int actualizarStockDB(sqlite3* db, int id, int cantidad);

/**
 * Imprime el listado de productos activos desde la base de datos.
 * @param db Puntero a la base de datos.
 */
void listarProductosDB(sqlite3* db);

/**
 * Menú interactivo para agregar un nuevo producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuAgregarProductoDB(sqlite3* db);

/**
 * Menú interactivo para actualizar el stock de un producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuActualizarStockDB(sqlite3* db);

/**
 * Menú interactivo para modificar un producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuModificarProductoDB(sqlite3* db);

#endif
