#ifndef DB_INIT_H
#define DB_INIT_H

#include <sqlite3.h>

/**
 * Inicializa la base de datos, creando las tablas necesarias y configurando las opciones.
 * @param db Puntero a la base de datos abierta.
 * @return 0 en caso de éxito, o un valor negativo en caso de error.
 */
int inicializarBaseDatos(sqlite3* db);

#endif
