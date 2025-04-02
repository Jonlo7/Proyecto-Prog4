#include <stdio.h>
#include <string.h>
#include "login/login.h"
#include "usuarios/usuarios.h"


Usuario* realizarLogin(RegistroUsuarios* reg) {
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
    
    return autenticarUsuario(reg, usuario, contrasena);
}

void menuLogin(RegistroUsuarios* reg) {
    Usuario* user = realizarLogin(reg);
    if (user) {
        printf("\n \033[1;32mLogin exitoso. Bienvenido %s.\033[0m\n", user->usuario);
    } else {
        printf("\n \033[1;31mError de login. Credenciales incorrectas.\033[0m\n");
    }
}