#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "colors.h"

#define MAX_NOMBRE 50
#define PRODUCTOS_FILE "../data/productos.txt"

typedef struct {
    int id;
    char nombre[MAX_NOMBRE+1];
    float precio;
    int stock;
    bool activo;
}Producto;

typedef struct {
    Producto* productos;   
    size_t cantidad;      
    size_t capacidad;     
} Inventario;

/**
 * Crea e inicializa un inventario.
 * @return Retorna un puntero al inventario creado o NULL en caso de error.
 */
Inventario* crearInventario(void);

/**
 * Libera la memoria asignada al inventario.
 * @param inv Puntero al inventario a liberar.
 */
void liberarInventario(Inventario* inv);

/**
 * Carga el inventario desde un fichero de texto.
 * @param inv Puntero al inventario a cargar.
 * @param filename Nombre del fichero desde el cual cargar el inventario.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int cargarInventario(Inventario* inv);

/**
 * Guarda el inventario en un fichero de texto, sobreescribiendo el contenido anterior.
 * @param inv Puntero al inventario a guardar.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int guardarInventario(const Inventario* inv);

/**
 * Agrega un nuevo producto al inventario.
 * @param inv Puntero al inventario.
 * @param nombre Nombre del producto.
 * @param precio Precio del producto.
 * @param stock Stock del producto.
 * @param activo Estado activo del producto (true o false).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int agregarProducto(Inventario* inv, const char* nombre, float precio, int stock, bool activo);

/**
 * Busca un producto por id en el inventario.
 * @param inv Puntero al inventario.
 * @param id ID del producto a buscar.
 * @return Retorna un puntero al producto encontrado o NULL si no se encuentra.
 */
Producto* buscarProducto(Inventario* inv, int id);

/**
 * Modifica los datos de un producto existente.
 * @param inv Puntero al inventario.
 * @param id ID del producto a modificar.
 * @param nuevoNombre Nuevo nombre del producto (puede ser NULL para no modificarlo).
 * @param nuevoPrecio Nuevo precio del producto (puede ser -1 para no modificarlo).
 * @param nuevoStock Nuevo stock del producto (puede ser -1 para no modificarlo).
 * @param nuevoActivo Nuevo estado activo del producto (puede ser -1 para no modificarlo).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int modificarProducto(Inventario* inv, int id, const char* nuevoNombre, float nuevoPrecio, int nuevoStock, bool nuevoActivo);

/**
 * Actualiza el stock de un producto (por ejemplo, en operaciones de venta o compra).
 * @param inv Puntero al inventario.
 * @param id ID del producto a actualizar.
 * @param cantidad Cantidad a agregar (puede ser negativa para restar).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int actualizarStock(Inventario* inv, int id, int cantidad);

/**
 * Imprime el listado de productos activos del inventario.
 * @param inv Puntero al inventario.
 */
void listarProductos(const Inventario* inv);

/**
 * Menú interactivo para agregar un nuevo producto.
 */
void menuAgregarProducto(Inventario* inv);

/**
 * Menú interactivo para actualizar el stock de un producto.
 */
void menuActualizarStock(Inventario* inv);

/**
 * Menú interactivo para modificar un producto.
 */
void menuModificarProducto(Inventario* inv);

#endif