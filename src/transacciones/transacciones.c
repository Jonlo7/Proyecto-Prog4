#include "transacciones.h"

Transaccion* crearTransaccion(void) {
    Transaccion* trans = malloc(sizeof(Transaccion));
    if (!trans) return NULL;
    trans->numItems = 0;
    trans->capacidadItems = 5;
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

/**
 * Guarda la transacción en la base de datos SQLite.
 * Se inserta primero la transacción en la tabla "transacciones" y luego cada uno de sus ítems en "items_transaccion".
 * @param db Puntero a la base de datos.
 * @param trans Puntero a la transacción.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int guardarTransaccionDB(sqlite3* db, const Transaccion* trans) {
    if (!db || !trans) return -1;
    
    const char* sqlTrans = "INSERT INTO transacciones (tipo, fecha, total) VALUES (?, ?, ?);";
    sqlite3_stmt* stmtTrans;
    int rc = sqlite3_prepare_v2(db, sqlTrans, -1, &stmtTrans, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando INSERT transaccion: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmtTrans, 1, (trans->tipo == VENTA) ? "VENTA" : "COMPRA", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmtTrans, 2, trans->fecha, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmtTrans, 3, trans->totalTransaccion);
    
    rc = sqlite3_step(stmtTrans);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error insertando transaccion: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmtTrans);
        return -1;
    }
    sqlite3_finalize(stmtTrans);
    
    long transaccionId = sqlite3_last_insert_rowid(db);
    
    const char* sqlItem = "INSERT INTO items_transaccion (transaccion_id, producto_id, cantidad, precio_unitario, total_item) "
                          "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmtItem;
    rc = sqlite3_prepare_v2(db, sqlItem, -1, &stmtItem, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando INSERT item: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    for (int i = 0; i < trans->numItems; i++) {
        sqlite3_bind_int(stmtItem, 1, (int)transaccionId);
        sqlite3_bind_int(stmtItem, 2, trans->items[i].producto.id);
        sqlite3_bind_int(stmtItem, 3, trans->items[i].cantidadTransaccion);
        sqlite3_bind_double(stmtItem, 4, trans->items[i].producto.precio);
        sqlite3_bind_double(stmtItem, 5, trans->items[i].totalItem);
        
        rc = sqlite3_step(stmtItem);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error insertando item de transaccion: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmtItem);
            return -1;
        }
        sqlite3_reset(stmtItem);
    }
    sqlite3_finalize(stmtItem);
    
    return 0;
}

/**
 * Menú interactivo para crear una transacción utilizando la base de datos.
 * @param inv Puntero al inventario.
 * @param db Puntero a la base de datos.
 */
void menuCrearTransaccionDB(Inventario* inv, sqlite3* db) {
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
    int resultado = scanf("%d", &opcionTipo);
    if (resultado != 1) {
        while (getchar() != '\n'); 
        printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        liberarTransaccion(trans);
        return;
    }
    if (opcionTipo < 1 || opcionTipo > 2) {
        printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
        liberarTransaccion(trans);
        return;
    }
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
        
        printf("¿Desea agregar otro producto? (1: Si, 0: No): ");
        resultado = scanf("%d", &continuar);
        if (resultado != 1) {
            while(getchar() != '\n'); 
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue; 
        }
    }
    
    calcularTotalTransaccion(trans);
    if (guardarTransaccionDB(db, trans) == 0)
        printf("\n \033[1;32mTransaccion guardada exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al guardar la transaccion.\033[0m\n");
    
    liberarTransaccion(trans);
}