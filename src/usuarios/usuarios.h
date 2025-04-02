#ifndef USUARIOS_H
#define USUARIOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "colors/colors.h"

#define MAX_USUARIO 50
#define MAX_CONTRASENA     50
#define USUARIOS_FILE     "../data/usuarios.txt"

typedef enum {EMPLEADO ,ADMINISTRADOR} Rol;

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
 * Crea e inicializa un registro de usuarios.
 * @return RegistroUsuarios* Retorna un puntero al registro creado o NULL en caso de error.
 */
RegistroUsuarios* crearRegistroUsuarios(void);

/**
 * Libera la memoria asignada al registro de usuarios.
 * @param reg Puntero al registro a liberar.
 */
void liberarRegistroUsuarios(RegistroUsuarios* reg);

/**
 * Carga los usuarios desde el fichero definido en USUARIOS_FILE.
 * @param reg Puntero al registro donde se cargarán los usuarios.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int cargarUsuarios(RegistroUsuarios* reg);

/**
 * Guarda los usuarios en el fichero definido en USUARIOS_FILE.
 * @param reg Puntero al registro de usuarios a guardar.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int guardarUsuarios(const RegistroUsuarios* reg);

/**
 * Registra un nuevo usuario en el registro.
 * @param reg Puntero al registro de usuarios.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña del usuario.
 * @param rol Rol del usuario (EMPLEADO o ADMINISTRADOR).
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int registrarUsuario(RegistroUsuarios* reg, const char* usuario, const char* contrasena, Rol rol);

/**
 * Autentica un usuario, solicitando nombre y contraseña.
 * @param reg Puntero al registro de usuarios.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña ingresada.
 * @return Usuario* Retorna un puntero al usuario autenticado o NULL si la autenticación falla.
 */
Usuario* autenticarUsuario(const RegistroUsuarios* reg, const char* usuario, const char* contrasena);

/**
 * Menú interactivo para registrar un nuevo usuario.
 * @param reg Puntero al registro de usuarios.
 */
void menuRegistrarUsuario(RegistroUsuarios* reg);

#endif