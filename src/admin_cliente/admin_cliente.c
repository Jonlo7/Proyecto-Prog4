#include "admin_cliente/admin_cliente.h"
#include "bbdd/sqlite/sqlite3.h"

void menuAdminClientDB(sqlite3* db) {
    int opcion = 0;
    do {
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|       ADMINISTRACION AVANZADA        |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Mostrar informes                  |\n");
        printf("| 2. Estadisticas                      |\n");
        printf("| 3. Opciones futuras                  |\n");
        printf("| 4. Salir                             |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        switch(opcion) {
            case 1:
                printf("\n \033[1;32mFuncionalidad de informes pendiente de implementacion.\033[0m\n");
                break;
            case 2:
                printf("\n \033[1;32mEstadisticas pendiente de implementacion.\033[0m\n");
                break;
            case 3:
                printf("\n \033[1;32mOtras opciones se implementaran en fases posteriores.\033[0m\n");
                break;
            case 4:
                printf("\n Saliendo del módulo de administracion...\n");
                break;
            default:
                printf("\n \033[1;31mOpción inválida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 4);
}
