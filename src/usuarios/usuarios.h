#ifndef USUARIOS_H
#define USUARIOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "colors/colors.h"
#include <sqlite3.h>

#define MAX_USUARIO 50
#define MAX_CONTRASENA 50

typedef enum {EMPLEADO, ADMINISTRADOR} Rol;

typedef struct {
    int id;
    char usuario[MAX_USUARIO];
    char contrasena[MAX_CONTRASENA];
    Rol rol;
} Usuario;

typedef struct {
    Usuario *usuarios;                  
    int cantidad;                        
    int capacidad;                       
} RegistroUsuarios;

/**
 * Crea e inicializa un registro de usuarios en memoria.
 * @return RegistroUsuarios* Retorna un puntero al registro creado o NULL en caso de error.
 */
RegistroUsuarios* crearRegistroUsuarios(void);

/**
 * Libera la memoria asignada al registro de usuarios.
 * @param reg Puntero al registro a liberar.
 */
void liberarRegistroUsuarios(RegistroUsuarios* reg);

/**
 * Carga los usuarios desde la base de datos SQLite.
 * @param reg Puntero al registro donde se cargarán los usuarios.
 * @param db Puntero a la base de datos.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int cargarUsuariosDB(RegistroUsuarios* reg, sqlite3* db);

/**
 * Registra un nuevo usuario en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña del usuario.
 * @param rol Rol del usuario (EMPLEADO o ADMINISTRADOR).
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int registrarUsuarioDB(sqlite3* db, const char* usuario, const char* contrasena, Rol rol);

/**
 * Autentica un usuario en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña ingresada.
 * @return Usuario* Retorna un puntero al usuario autenticado o NULL si la autenticación falla.
 */
Usuario* autenticarUsuarioDB(sqlite3* db, const char* usuario, const char* contrasena);

/**
 * Menú interactivo para registrar un nuevo usuario utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuRegistrarUsuarioDB(sqlite3* db);

#endif