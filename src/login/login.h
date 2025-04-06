#ifndef LOGIN_H
#define LOGIN_H

#include "usuarios/usuarios.h"
#include "colors/colors.h"
#include "bbdd/sqlite/sqlite3.h"

/**
 * Realiza el proceso de login solicitando usuario y contraseña utilizando la base de datos.
 * @param reg Puntero al registro de usuarios.
 * @param db Puntero a la base de datos.
 * @return Usuario* Retorna un puntero al usuario autenticado o NULL si la autenticación falla.
 */
Usuario* realizarLoginDB(RegistroUsuarios* reg, sqlite3* db);

/**
 * Menú interactivo para el login utilizando la base de datos.
 * @param reg Puntero al registro de usuarios.
 * @param db Puntero a la base de datos.
 */
void menuLoginDB(RegistroUsuarios* reg, sqlite3* db);

#endif
