#include <stdio.h>
#include <string.h>
#include "login/login.h"
#include "usuarios/usuarios.h"
#include "bbdd/sqlite/sqlite3.h"

Usuario* realizarLoginDB(RegistroUsuarios* reg, sqlite3* db) {
    char usuario[MAX_USUARIO + 1];
    char contrasena[MAX_CONTRASENA + 1];
    
    printf("\n\033[1;35m");
    printf("+---------------------------------+\n");
    printf("|              LOGIN              |\n");
    printf("+---------------------------------+\n");
    printf("\033[0m");
    printf("Ingrese el nombre de usuario: ");
    scanf(" %[^\n]", usuario);
    printf("Ingrese la contrasenya: ");
    scanf(" %[^\n]", contrasena);
    
    // Se utiliza la versión DB para autenticar, que debería estar implementada en usuarios/usuarios.c
    return autenticarUsuarioDB(db, usuario, contrasena);
}

void menuLoginDB(RegistroUsuarios* reg, sqlite3* db) {
    Usuario* user = realizarLoginDB(reg, db);
    if (user) {
        printf("\n \033[1;32mLogin exitoso. Bienvenido %s.\033[0m\n", user->usuario);
    } else {
        printf("\n \033[1;31mError de login. Credenciales incorrectas.\033[0m\n");
    }
}
