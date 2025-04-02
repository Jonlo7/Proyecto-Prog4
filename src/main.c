#include <stdio.h>
#include "inventario/inventario.h"
#include "usuarios/usuarios.h"
#include "transacciones/transacciones.h"
#include "login/login.h"
#include "colors/colors.h"

void menuAdmin(RegistroUsuarios* reg, Inventario* inv) {
    int opcion = 0;
    do {
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|         MENU ADMINISTRADOR           |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Listar productos                  |\n");
        printf("| 2. Agregar producto                  |\n");
        printf("| 3. Actualizar stock                  |\n");
        printf("| 4. Modificar producto                |\n");
        printf("| 5. Crear transaccion                 |\n");
        printf("| 6. Registrar usuario                 |\n");
        printf("| 7. Cerrar sesión                     |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        switch(opcion) {
            case 1:
                listarProductos(inv);
                break;
            case 2:
                menuAgregarProducto(inv);
                break;
            case 3:
                menuActualizarStock(inv);
                break;
            case 4:
                menuModificarProducto(inv);
                break;
            case 5:
                menuCrearTransaccion(inv);
                break;
            case 6:
                menuRegistrarUsuario(reg);
                break;
            case 7:
                printf("Cerrando sesion...\n");
                break;
            default:
                printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 7);
}

void menuEmpleado(Inventario* inv) {
    int opcion = 0;
    do {
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|            MENU EMPLEADO             |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Listar productos                  |\n");
        printf("| 2. Actualizar stock                  |\n");
        printf("| 3. Crear transaccion                 |\n");
        printf("| 4. Cerrar sesión                     |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        switch(opcion) {
            case 1:
                listarProductos(inv);
                break;
            case 2:
                menuActualizarStock(inv);
                break;
            case 3:
                menuCrearTransaccion(inv);
                break;
            case 4:
                printf("Cerrando sesion...\n");
                break;
            default:
                printf("\033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 4);
}

int main(void) {
    Inventario* inv = crearInventario();
    if (!inv) {
        printf("\n \033[1;31mError al crear el inventario.\033[0m\n");
        return 1;
    }
    if (cargarInventario(inv) != 0) {
        printf("\n \033[1;31mError al cargar el inventario.\033[0m\n");
    }
    
    RegistroUsuarios* reg = crearRegistroUsuarios();
    if (!reg) {
        printf("\n \033[1;31mError al crear el registro de usuarios.\033[0m\n");
        liberarInventario(inv);
        return 1;
    }
    if (cargarUsuarios(reg) != 0) {
        printf("\n \033[1;31mError al cargar los usuarios.\033[0m\n");
    }
    
    int opcion = 0;
    do {
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|      SISTEMA GESTOR DE INVENTARIO    |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Login                             |\n");
        printf("| 2. Registrar usuario                 |\n");
        printf("| 3. Salir                             |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        
        switch(opcion) {
            case 1: {
                Usuario* user = realizarLogin(reg);
                if (user) {
                    printf("\n \033[1;32mLogin exitoso. Bienvenido %s.\033[0m\n", user->usuario);
                    if (user->rol == ADMINISTRADOR)
                        menuAdmin(reg, inv);
                    else
                        menuEmpleado(inv);
                } else {
                    printf("\n \033[1;31mError de login. Credenciales incorrectas.\033[0m\n");
                }
                break;
            }
            case 2:
                menuRegistrarUsuario(reg);
                break;
            case 3:
                printf("Saliendo de la aplicacion...\n");
                break;
            default:
                printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 3);
    
    if (guardarInventario(inv) != 0) {
        printf("\n \033[1;31mError al guardar el inventario.\033[0m\n");
    }
    if (guardarUsuarios(reg) != 0) {
        printf("\n \033[1;31mError al guardar los usuarios.\033[0m\n");
    }
    
    liberarRegistroUsuarios(reg);
    liberarInventario(inv);
    
    return 0;
}
