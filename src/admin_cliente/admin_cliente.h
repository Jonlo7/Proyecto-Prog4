#ifndef ADMIN_CLIENT_H
#define ADMIN_CLIENT_H

#include <stdio.h>
#include "colors/colors.h"
#include <sqlite3.h>

/**
 * Función para solicitar un informe, que podría utilizar la BD para generar datos.
 */
void solicitarInformeDB(sqlite3* db);

/**
 * Menú interactivo para el módulo de administración avanzada utilizando la base de datos.
 */
void menuAdminClientDB(sqlite3* db);

#endif
