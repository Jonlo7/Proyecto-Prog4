#ifndef LOGIN_H
#define LOGIN_H

#include "usuarios.h"
#include "colors.h"

/**
 * Realiza el proceso de login solicitando usuario y contraseña.
 * @param reg Puntero al registro de usuarios.
 * @return Usuario* Retorna un puntero al usuario autenticado o NULL si la autenticación falla.
 */
Usuario* realizarLogin(RegistroUsuarios* reg);

/**
 * Menú interactivo para el login.
 * @param reg Puntero al registro de usuarios.
 */
void menuLogin(RegistroUsuarios* reg);

#endif 
