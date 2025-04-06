#include <stdio.h>
#include "db_init.h"

int inicializarBaseDatos(sqlite3* db) {
    int rc;
    char *errMsg = NULL;

    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error habilitando foreign_keys: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1;
    }
    
    const char* sqlProductos =
        "CREATE TABLE IF NOT EXISTS productos ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nombre TEXT NOT NULL,"
        "  precio REAL NOT NULL,"
        "  stock INTEGER NOT NULL,"
        "  activo INTEGER NOT NULL"
        ");";
    rc = sqlite3_exec(db, sqlProductos, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla productos: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1;
    }

    const char* sqlUsuarios =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  usuario TEXT NOT NULL,"
        "  contrasena TEXT NOT NULL,"
        "  rol INTEGER NOT NULL"
        ");";
    rc = sqlite3_exec(db, sqlUsuarios, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla usuarios: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1;
    }

    const char* sqlTransacciones =
        "CREATE TABLE IF NOT EXISTS transacciones ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  tipo TEXT NOT NULL,"
        "  fecha TEXT NOT NULL,"
        "  total REAL NOT NULL"
        ");";
    rc = sqlite3_exec(db, sqlTransacciones, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla transacciones: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1;
    }

    const char* sqlItems =
        "CREATE TABLE IF NOT EXISTS items_transaccion ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  transaccion_id INTEGER NOT NULL,"
        "  producto_id INTEGER NOT NULL,"
        "  cantidad INTEGER NOT NULL,"
        "  precio_unitario REAL NOT NULL,"
        "  total_item REAL NOT NULL,"
        "  FOREIGN KEY (transaccion_id) REFERENCES transacciones(id),"
        "  FOREIGN KEY (producto_id) REFERENCES productos(id)"
        ");";
    rc = sqlite3_exec(db, sqlItems, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla items_transaccion: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1;
    }
    
    return 0;
}
