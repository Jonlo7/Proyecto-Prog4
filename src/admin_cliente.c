#include "admin_cliente.h"

void menuAdminClient(void) {
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
                printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        }
    } while(opcion != 4);
}