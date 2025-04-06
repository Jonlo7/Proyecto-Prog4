#include "inventario/inventario.h"
#include "usuarios/usuarios.h"
#include "transacciones/transacciones.h"
#include "login/login.h"
#include "colors/colors.h"
#include "bbdd/sqlite/sqlite3.h"
#include "bbdd/db_init/db_init.h"

void menuAdminDB(RegistroUsuarios* reg, Inventario* inv, sqlite3* db) {
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
        printf("| 7. Cerrar sesion                     |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opcion: ");
        int resultado = scanf("%d", &opcion);
        if (resultado != 1) {
            while (getchar() != '\n'); 
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue; 
        }
        if (opcion < 1 || opcion > 7) {
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue;
        }
        switch(opcion) {
            case 1:
                listarProductosDB(db);
                break;
            case 2:
                menuAgregarProductoDB(db);
                break;
            case 3:
                menuActualizarStockDB(db);
                break;
            case 4:
                menuModificarProductoDB(db);
                break;
            case 5:
                menuCrearTransaccionDB(inv, db);
                break;
            case 6:
                menuRegistrarUsuarioDB(db);
                break;
            case 7:
                printf("Cerrando sesion...\n");
                break;
            default:
                printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 7);
}

void menuEmpleadoDB(Inventario* inv, sqlite3* db) {
    int opcion = 0;
    do {
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|            MENU EMPLEADO             |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Listar productos                  |\n");
        printf("| 2. Actualizar stock                  |\n");
        printf("| 3. Crear transaccion                 |\n");
        printf("| 4. Cerrar sesion                     |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opcion: ");
        int resultado = scanf("%d", &opcion);
        if (resultado != 1) {
            while (getchar() != '\n'); 
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue; 
        }
        if (opcion < 1 || opcion > 4) {
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue;
        }
        switch(opcion) {
            case 1:
                listarProductosDB(db);
                break;
            case 2:
                menuActualizarStockDB(db);
                break;
            case 3:
                menuCrearTransaccionDB(inv, db);
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
    sqlite3* db;
    int rc = sqlite3_open("inventario.sqlite", &db);
    if (rc != SQLITE_OK) {
        printf("\n \033[1;31mError al abrir la base de datos: %s\033[0m\n", sqlite3_errmsg(db));
        return 1;
    }
    
    if (inicializarBaseDatos(db) != 0) {
        printf("\n \033[1;31mError al inicializar la base de datos.\033[0m\n");
        sqlite3_close(db);
        return 1;
    }
    
    Inventario* inv = crearInventario();
    if (!inv) {
        printf("\n \033[1;31mError al crear el inventario.\033[0m\n");
        sqlite3_close(db);
        return 1;
    }
    if (cargarInventarioDB(inv, db) != 0) {
        printf("\n \033[1;31mError al cargar el inventario.\033[0m\n");
    }
    
    RegistroUsuarios* reg = crearRegistroUsuarios();
    if (!reg) {
        printf("\n \033[1;31mError al crear el registro de usuarios.\033[0m\n");
        liberarInventario(inv);
        sqlite3_close(db);
        return 1;
    }
    if (cargarUsuariosDB(reg, db) != 0) {
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
        int resultado = scanf("%d", &opcion);
        if (resultado != 1) {
            while (getchar() != '\n'); 
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue; 
        }
        if (opcion < 1 || opcion > 3) {
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue;
        }
        
        switch(opcion) {
            case 1: {
                Usuario* user = realizarLoginDB(reg, db);
                if (user) {
                    printf("\n \033[1;32mLogin exitoso. Bienvenido %s.\033[0m\n", user->usuario);
                    if (user->rol == ADMINISTRADOR)
                        menuAdminDB(reg, inv, db);
                    else
                        menuEmpleadoDB(inv, db);
                } else {
                    printf("\n \033[1;31mError de login. Credenciales incorrectas.\033[0m\n");
                }
                break;
            }
            case 2:
                menuRegistrarUsuarioDB(db);
                break;
            case 3:
                printf("Saliendo de la aplicacion...\n");
                break;
            default:
                printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
                continue;
        }
    } while(opcion != 3);
    
    liberarRegistroUsuarios(reg);
    liberarInventario(inv);
    sqlite3_close(db);
    
    return 0;
}
