#include "transacciones/transacciones.h"

Transaccion* crearTransaccion(void) {
    Transaccion* trans = malloc(sizeof(Transaccion));
    if (!trans) return NULL;
    trans->numItems = 0;
    trans->capacidadItems = 5; // Capacidad inicial para 5 ítems
    trans->items = malloc(trans->capacidadItems * sizeof(ItemTransaccion));
    if (!trans->items) {
        free(trans);
        return NULL;
    }
    trans->totalTransaccion = 0.0f;
    trans->fecha[0] = '\0';
    return trans;
}

void liberarTransaccion(Transaccion* trans) {
    if (trans) {
        free(trans->items);
        free(trans);
    }
}

int agregarItemTransaccion(Transaccion* trans, Producto prod, int cantidad) {
    if (!trans) return -1;
    if (trans->numItems == trans->capacidadItems) {
        int nuevaCapacidad = trans->capacidadItems * 2;
        ItemTransaccion* temp = realloc(trans->items, nuevaCapacidad * sizeof(ItemTransaccion));
        if (!temp) return -1;
        trans->items = temp;
        trans->capacidadItems = nuevaCapacidad;
    }
    ItemTransaccion item;
    item.producto = prod;
    item.cantidadTransaccion = cantidad;
    item.totalItem = prod.precio * cantidad;
    trans->items[trans->numItems++] = item;
    return 0;
}

void calcularTotalTransaccion(Transaccion* trans) {
    if (!trans) return;
    float total = 0.0f;
    for (int i = 0; i < trans->numItems; i++) {
        total += trans->items[i].totalItem;
    }
    trans->totalTransaccion = total;
}

int guardarTransaccion(const Transaccion* trans) {
    if (!trans) return -1;
    FILE* file = fopen(TRANSACCIONES_FILE, "a");
    if (!file) return -1;
    
    const char* tipoStr = (trans->tipo == VENTA) ? "VENTA" : "COMPRA";
    fprintf(file, "%s|%s|%.2f|", tipoStr, trans->fecha, trans->totalTransaccion);
    
    for (int i = 0; i < trans->numItems; i++) {
        const ItemTransaccion* item = &trans->items[i];
        fprintf(file, "%d,%s,%d,%.2f,%.2f", 
                item->producto.id, item->producto.nombre, 
                item->cantidadTransaccion, item->producto.precio, item->totalItem);
        if (i < trans->numItems - 1)
            fprintf(file, ";");
    }
    fprintf(file, "\n");
    fclose(file);
    return 0;
}

void menuCrearTransaccion(Inventario* inv) {
    if (!inv) {
        printf("\033[1;31mInventario no disponible.\033[0m\n");
        return;
    }
    
    Transaccion* trans = crearTransaccion();
    if (!trans) {
        printf("\033[1;31mError al crear la transaccion.\033[0m\n");
        return;
    }
    
    int opcionTipo;
    printf("\n\033[1;35m");
    printf("+--------------------------------------+\n");
    printf("|         CREAR TRANSACCION            |\n");
    printf("+--------------------------------------+\n");
    printf("\033[0m");
    printf("Seleccione el tipo de transaccion:\n");
    printf("1. Venta\n");
    printf("2. Compra\n");
    printf("Ingrese su opcion: ");
    scanf("%d", &opcionTipo);
    trans->tipo = (opcionTipo == 1) ? VENTA : COMPRA;
    
    printf("Ingrese la fecha (YYYY-MM-DD): ");
    scanf("%s", trans->fecha);
    
    int continuar = 1;
    while (continuar) {
        int id, cantidad;
        printf("\n\033[1;35m");
        printf("+--------------------------------------+\n");
        printf("|    AGREGAR ITEM A LA TRANSACCION     |\n");
        printf("+--------------------------------------+\n");
        printf("\033[0m");
        printf("Ingrese el ID del producto: ");
        scanf("%d", &id);
        
        Producto* prod = buscarProducto(inv, id);
        if (!prod) {
            printf("\033[1;31mProducto con ID %d no encontrado.\033[0m\n", id);
        } else {
            printf("Ingrese la cantidad a transaccionar: ");
            scanf("%d", &cantidad);
            if (agregarItemTransaccion(trans, *prod, cantidad) == 0)
                printf("\n \033[1;32mItem agregado exitosamente.\033[0m\n");
            else
                printf("\n \033[1;31mError al agregar el item.\033[0m\n");
        }
        
        printf("¿Desea agregar otro producto? (1: Sí, 0: No): ");
        scanf("%d", &continuar);
    }

    calcularTotalTransaccion(trans);
    if (guardarTransaccion(trans) == 0)
        printf("\n \033[1;32mTransaccion guardada exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al guardar la transaccion.\033[0m\n");
    
    liberarTransaccion(trans);
}