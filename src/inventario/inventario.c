#include "inventario.h"
#include "bbdd/sqlite/sqlite3.h"

Inventario* crearInventario(void) {
    Inventario* inv = malloc(sizeof(Inventario));
    if (!inv) return NULL;
    inv->cantidad = 0;
    inv->capacidad = 10; 
    inv->productos = malloc(inv->capacidad * sizeof(Producto));
    if (!inv->productos) {
        free(inv);
        return NULL;
    }
    return inv;
}

void liberarInventario(Inventario* inv) {
    if (inv) {
        free(inv->productos);
        free(inv);
    }
}

/**
 * Carga el inventario desde la base de datos SQLite.
 * @param inv Puntero al inventario a cargar.
 * @param db Puntero a la base de datos.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int cargarInventarioDB(Inventario* inv, sqlite3* db) {
    if (!inv || !db) return -1;
    const char* sql = "SELECT id, nombre, precio, stock, activo FROM productos;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    inv->cantidad = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Producto prod;
        prod.id = sqlite3_column_int(stmt, 0);
        const unsigned char* nombre = sqlite3_column_text(stmt, 1);
        strncpy(prod.nombre, (const char*)nombre, MAX_NOMBRE);
        prod.nombre[MAX_NOMBRE] = '\0';
        prod.precio = (float)sqlite3_column_double(stmt, 2);
        prod.stock = sqlite3_column_int(stmt, 3);
        prod.activo = sqlite3_column_int(stmt, 4) != 0;
        if (inv->cantidad == inv->capacidad) {
            size_t nuevaCapacidad = inv->capacidad * 2;
            Producto* temp = realloc(inv->productos, nuevaCapacidad * sizeof(Producto));
            if (!temp) {
                sqlite3_finalize(stmt);
                return -1;
            }
            inv->productos = temp;
            inv->capacidad = nuevaCapacidad;
        }
        inv->productos[inv->cantidad++] = prod;
    }
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Inserta un nuevo producto en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param nombre Nombre del producto.
 * @param precio Precio del producto.
 * @param stock Stock del producto.
 * @param activo Estado activo del producto (true o false).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int insertarProductoDB(sqlite3* db, const char* nombre, float precio, int stock, bool activo) {
    if (!db || !nombre) return -1;
    const char* sql = "INSERT INTO productos (nombre, precio, stock, activo) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando INSERT: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, precio);
    sqlite3_bind_int(stmt, 3, stock);
    sqlite3_bind_int(stmt, 4, activo ? 1 : 0);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error insertando producto: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Busca un producto por id en el inventario (en memoria).
 * @param inv Puntero al inventario.
 * @param id ID del producto a buscar.
 * @return Retorna un puntero al producto encontrado o NULL si no se encuentra.
 */
Producto* buscarProducto(Inventario* inv, int id) {
    if (!inv) return NULL;
    for (size_t i = 0; i < inv->cantidad; i++) {
        if (inv->productos[i].id == id)
            return &inv->productos[i];
    }
    return NULL;
}

/**
 * Modifica los datos de un producto existente en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param id ID del producto a modificar.
 * @param nuevoNombre Nuevo nombre del producto (puede ser NULL para no modificarlo).
 * @param nuevoPrecio Nuevo precio del producto.
 * @param nuevoStock Nuevo stock del producto.
 * @param nuevoActivo Nuevo estado activo del producto.
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int modificarProductoDB(sqlite3* db, int id, const char* nuevoNombre, float nuevoPrecio, int nuevoStock, bool nuevoActivo) {
    const char* sql = "UPDATE productos SET nombre = ?, precio = ?, stock = ?, activo = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando UPDATE: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    if (nuevoNombre)
        sqlite3_bind_text(stmt, 1, nuevoNombre, -1, SQLITE_STATIC);
    else
        sqlite3_bind_null(stmt, 1);
    sqlite3_bind_double(stmt, 2, nuevoPrecio);
    sqlite3_bind_int(stmt, 3, nuevoStock);
    sqlite3_bind_int(stmt, 4, nuevoActivo ? 1 : 0);
    sqlite3_bind_int(stmt, 5, id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error actualizando producto: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Actualiza el stock de un producto en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param id ID del producto a actualizar.
 * @param cantidad Cantidad a agregar (puede ser negativa para restar).
 * @return Retorna 0 en caso de éxito o un valor distinto en caso de error.
 */
int actualizarStockDB(sqlite3* db, int id, int cantidad) {
    const char* sqlSelect = "SELECT stock FROM productos WHERE id = ?;";
    sqlite3_stmt* stmtSelect;
    int rc = sqlite3_prepare_v2(db, sqlSelect, -1, &stmtSelect, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando SELECT: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_int(stmtSelect, 1, id);
    rc = sqlite3_step(stmtSelect);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Producto con ID %d no encontrado.\n", id);
        sqlite3_finalize(stmtSelect);
        return -1;
    }
    int stockActual = sqlite3_column_int(stmtSelect, 0);
    sqlite3_finalize(stmtSelect);
    if (stockActual + cantidad < 0)
        return -1;
    
    const char* sqlUpdate = "UPDATE productos SET stock = ? WHERE id = ?;";
    sqlite3_stmt* stmtUpdate;
    rc = sqlite3_prepare_v2(db, sqlUpdate, -1, &stmtUpdate, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando UPDATE: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_int(stmtUpdate, 1, stockActual + cantidad);
    sqlite3_bind_int(stmtUpdate, 2, id);
    rc = sqlite3_step(stmtUpdate);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error actualizando stock: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmtUpdate);
        return -1;
    }
    sqlite3_finalize(stmtUpdate);
    return 0;
}

/**
 * Imprime el listado de productos activos desde la base de datos SQLite.
 * @param db Puntero a la base de datos.
 */
void listarProductosDB(sqlite3* db) {
    const char* sql = "SELECT id, nombre, precio, stock, activo FROM productos;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando SELECT: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("\n\033[1;35m"); 
    printf("+--------------------------------------+\n");
    printf("|       LISTADO DE PRODUCTOS           |\n");
    printf("+--------------------------------------+\n");
    printf("\033[0m");
    printf("ID\tNombre\t\t\tPrecio\tStock\tEstado\n");
    printf("-------------------------------------------------------\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* nombre = sqlite3_column_text(stmt, 1);
        float precio = (float)sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        const char* estado =sqlite3_column_int(stmt, 4) ? "Activo" : "Inactivo";
        printf("%d\t%-20s\t%.2f\t%d\t%s\n", id, nombre, precio, stock, estado);
    }
    sqlite3_finalize(stmt);
}

/*       MENUS       */

/**
 * Menú interactivo para agregar un nuevo producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuAgregarProductoDB(sqlite3* db) {
    char nombre[MAX_NOMBRE + 1];
    float precio;
    int stock;
    int activoInt;
    bool activo;

    printf("\n\033[1;35m");
    printf("+--------------------------------------+\n");
    printf("|           AGREGAR PRODUCTO           |\n");
    printf("+--------------------------------------+\n");
    printf("\033[0m");
    printf("Ingrese el nombre del producto: ");
    scanf(" %[^\n]", nombre);
    printf("Ingrese el precio: ");
    scanf("%f", &precio);
    printf("Ingrese el stock: ");
    scanf("%d", &stock);
    printf("¿Producto activo? (1 para si, 0 para no): ");
    scanf("%d", &activoInt);
    activo = (activoInt == 1);

    if (insertarProductoDB(db, nombre, precio, stock, activo) == 0)
        printf("\n \033[1;32mProducto agregado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al agregar el producto.\033[0m\n");
}

/**
 * Menú interactivo para actualizar el stock de un producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuActualizarStockDB(sqlite3* db) {
    int id, cantidad;
    printf("\n\033[1;35m");
    printf("+--------------------------------------+\n");
    printf("|          ACTUALIZAR STOCK            |\n");
    printf("+--------------------------------------+\n");
    printf("\033[0m");
    printf("Ingrese el ID del producto: ");
    scanf("%d", &id);
    printf("Ingrese la cantidad a agregar (use numero negativo para restar): ");
    scanf("%d", &cantidad);

    if (actualizarStockDB(db, id, cantidad) == 0)
        printf("\n \033[1;32mStock actualizado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al actualizar el stock.\033[0m\n");
}

/**
 * Menú interactivo para modificar un producto utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuModificarProductoDB(sqlite3* db) {
    int id;
    printf("Ingrese el ID del producto a modificar: ");
    scanf("%d", &id);
    
    Inventario* inv = crearInventario();
    if(cargarInventarioDB(inv, db) != 0) {
        printf("\n \033[1;31mError cargando inventario.\033[0m\n");
        liberarInventario(inv);
        return;
    }
    Producto* prod = buscarProducto(inv, id);
    if (!prod) {
         printf("\n \033[1;31mProducto con ID %d no encontrado.\033[0m\n", id);
         liberarInventario(inv);
         return;
    }
    
    int opcion;
    do {
         printf("\n\033[1;35m");
         printf("+--------------------------------------+\n");
         printf("|         MODIFICAR PRODUCTO           |\n");
         printf("+--------------------------------------+\n");
         printf("| 1. Cambiar precio                    |\n");
         printf("| 2. Cambiar stock                     |\n");
         printf("| 3. Cambiar nombre                    |\n");
         printf("| 4. Marcar como inactivo (baja)       |\n");
         printf("| 5. Terminar edicion y guardar        |\n");
         printf("+--------------------------------------+\n");
         printf("\033[0m");
         printf("Seleccione una opcion: ");
         int resultado = scanf("%d", &opcion);
        if (resultado != 1) {
            while (getchar() != '\n'); 
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue; 
        }
        if (opcion < 1 || opcion > 5) {
            printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
            continue;
        }
         switch(opcion) {
              case 1: {
                  float nuevoPrecio;
                  printf("Ingrese el nuevo precio: ");
                  scanf("%f", &nuevoPrecio);
                  prod->precio = nuevoPrecio;
                  printf("\n \033[1;32mPrecio actualizado en memoria.\033[0m\n");
              } break;
              case 2: {
                  int nuevoStock;
                  printf("Ingrese el nuevo stock: ");
                  scanf("%d", &nuevoStock);
                  prod->stock = nuevoStock;
                  printf("\n \033[1;32mStock actualizado en memoria.\033[0m\n");
              } break;
              case 3: {
                  char nuevoNombre[MAX_NOMBRE + 1];
                  printf("Ingrese el nuevo nombre: ");
                  scanf(" %[^\n]", nuevoNombre);
                  strncpy(prod->nombre, nuevoNombre, MAX_NOMBRE);
                  prod->nombre[MAX_NOMBRE] = '\0';
                  printf("\n \033[1;32mNombre actualizado en memoria.\033[0m\n");
              } break;
              case 4: {
                  prod->activo = false;
                  printf("\n \033[1;32mProducto marcado como inactivo en memoria.\033[0m\n");
              } break;
              case 5:
                  printf("\n Terminando edicion y guardando cambios en la BD...\n");
                  break;
              default:
                  printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
                  break;
         }
    } while(opcion != 5);
    
    if (modificarProductoDB(db, prod->id, prod->nombre, prod->precio, prod->stock, prod->activo) == 0)
        printf("\n \033[1;32mProducto actualizado en la BD exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al actualizar el producto en la BD.\033[0m\n");
    
    liberarInventario(inv);
}
