#include "inventario.h"

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

int cargarInventario(Inventario* inv) {
    if (!inv) return -1;
    FILE* file = fopen(PRODUCTOS_FILE, "r");
    if (!file) return -1;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '\r')
            continue;
        Producto prod;
        int activoInt;
        int items = sscanf(line, "%d|%50[^|]|%f|%d|%d", 
                           &prod.id, prod.nombre, &prod.precio, &prod.stock, &activoInt);
        if (items < 5)
            continue; 
        prod.activo = (activoInt != 0);
        
        if (inv->cantidad == inv->capacidad) {
            size_t nuevaCapacidad = inv->capacidad * 2;
            Producto* temp = realloc(inv->productos, nuevaCapacidad * sizeof(Producto));
            if (!temp) {
                fclose(file);
                return -1;
            }
            inv->productos = temp;
            inv->capacidad = nuevaCapacidad;
        }
        inv->productos[inv->cantidad++] = prod;
    }
    fclose(file);
    return 0;
}


int guardarInventario(const Inventario* inv) {
    if (!inv) return -1;
    FILE* file = fopen(PRODUCTOS_FILE, "w");
    if (!file) return -1;
    
    for (size_t i = 0; i < inv->cantidad; i++) {
        const Producto* prod = &inv->productos[i];
        fprintf(file, "%d|%s|%.2f|%d|%d\n", prod->id, prod->nombre, prod->precio, prod->stock, prod->activo ? 1 : 0);
    }
    fclose(file);
    return 0;
}

int agregarProducto(Inventario* inv, const char* nombre, float precio, int stock, bool activo) {
    if (!inv || !nombre) return -1;
    
    int nextId = 0;
    for (size_t i = 0; i < inv->cantidad; i++) {
        if (inv->productos[i].id > nextId)
            nextId = inv->productos[i].id;
    }
    nextId++;
    
    if (inv->cantidad == inv->capacidad) {
        size_t nuevaCapacidad = inv->capacidad * 2;
        Producto* temp = realloc(inv->productos, nuevaCapacidad * sizeof(Producto));
        if (!temp) return -1;
        inv->productos = temp;
        inv->capacidad = nuevaCapacidad;
    }
    
    Producto prod;
    prod.id = nextId;
    strncpy(prod.nombre, nombre, MAX_NOMBRE);
    prod.nombre[MAX_NOMBRE] = '\0';
    prod.precio = precio;
    prod.stock = stock;
    prod.activo = activo;
    
    inv->productos[inv->cantidad++] = prod;
    return 0;
}

Producto* buscarProducto(Inventario* inv, int id) {
    if (!inv) return NULL;
    for (size_t i = 0; i < inv->cantidad; i++) {
        if (inv->productos[i].id == id)
            return &inv->productos[i];
    }
    return NULL;
}

int modificarProducto(Inventario* inv, int id, const char* nuevoNombre, float nuevoPrecio, int nuevoStock, bool nuevoActivo) {
    Producto* prod = buscarProducto(inv, id);
    if (!prod) return -1; 
    if (nuevoNombre) {
        strncpy(prod->nombre, nuevoNombre, MAX_NOMBRE);
        prod->nombre[MAX_NOMBRE] = '\0';
    }
    prod->precio = nuevoPrecio;
    prod->stock = nuevoStock;
    prod->activo = nuevoActivo;
    return 0;
}

int actualizarStock(Inventario* inv, int id, int cantidad) {
    Producto* prod = buscarProducto(inv, id);
    if (!prod) return -1;  
    if (prod->stock + cantidad < 0)
        return -1; 
    prod->stock += cantidad;
    return 0;
}

void listarProductos(const Inventario* inv) {
    if (!inv) return;
    printf("\n\033[1;35m"); 
    printf("+--------------------------------------+\n");
    printf("|       LISTADO DE PRODUCTOS           |\n");
    printf("+--------------------------------------+\n");
    printf("\033[0m");
    printf("ID\tNombre\t\t\tPrecio\tStock\n");
    printf("-------------------------------------------------\n");
    for (size_t i = 0; i < inv->cantidad; i++) {
        const Producto* prod = &inv->productos[i];
        if (prod->activo) {
            printf("%d\t%-20s\t%.2f\t%d\n", prod->id, prod->nombre, prod->precio, prod->stock);
        }
    }
}

/*       MENUS       */

void menuAgregarProducto(Inventario* inv) {
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

    if (agregarProducto(inv, nombre, precio, stock, activo) == 0)
        printf("\n \033[1;32mProducto agregado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al agregar el producto.\033[0m\n");
}

void menuActualizarStock(Inventario* inv) {
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

    if (actualizarStock(inv, id, cantidad) == 0)
        printf("\n \033[1;32mStock actualizado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al actualizar el stock.\033[0m\n");
}

void menuModificarProducto(Inventario* inv) {
    int id;
    printf("Ingrese el ID del producto a modificar: ");
    scanf("%d", &id);
    
    Producto* prod = buscarProducto(inv, id);
    if (!prod) {
         printf("\n \033[1;31mProducto con ID %d no encontrado.\033[0m\n", id);
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
         printf("| 5. Terminar edición y guardar        |\n");
         printf("+--------------------------------------+\n");
         printf("\033[0m");
         printf("Seleccione una opcion: ");
         scanf("%d", &opcion);
         
         switch(opcion) {
              case 1:
                   {
                       float nuevoPrecio;
                       printf("Ingrese el nuevo precio: ");
                       scanf("%f", &nuevoPrecio);
                       prod->precio = nuevoPrecio;
                       printf("\n \033[1;32mPrecio actualizado.\033[0m\n");
                   }
                   break;
              case 2:
                   {
                       int nuevoStock;
                       printf("Ingrese el nuevo stock: ");
                       scanf("%d", &nuevoStock);
                       prod->stock = nuevoStock;
                       printf("\n \033[1;32mStock actualizado.\033[0m\n");
                   }
                   break;
              case 3:
                   {
                       char nuevoNombre[MAX_NOMBRE + 1];
                       printf("Ingrese el nuevo nombre: ");
                       scanf(" %[^\n]", nuevoNombre);
                       strncpy(prod->nombre, nuevoNombre, MAX_NOMBRE);
                       prod->nombre[MAX_NOMBRE] = '\0';
                       printf("\n \033[1;32mNombre actualizado.\033[0m\n");
                   }
                   break;
              case 4:
                   {
                       prod->activo = false;
                       printf("\n \033[1;32mProducto marcado como inactivo.\033[0m\n");
                   }
                   break;
              case 5:
                   printf("\n Terminando edicion y guardando...\n");
                   break;
              default:
                   printf("\n \033[1;31mOpcion invalida. Intente de nuevo.\033[0m\n");
                   break;
         }
    } while(opcion != 5);
}