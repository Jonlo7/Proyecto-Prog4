#include "usuarios.h"
#include "bbdd/sqlite/sqlite3.h"

RegistroUsuarios* crearRegistroUsuarios(void) {
    RegistroUsuarios* reg = malloc(sizeof(RegistroUsuarios));
    if (!reg) return NULL;
    reg->cantidad = 0;
    reg->capacidad = 5;
    reg->usuarios = malloc(reg->capacidad * sizeof(Usuario));
    if (!reg->usuarios) {
        free(reg);
        return NULL;
    }
    return reg;
}

void liberarRegistroUsuarios(RegistroUsuarios* reg) {
    if (reg) {
        free(reg->usuarios);
        free(reg);
    }
}

/**
 * Carga los usuarios desde la base de datos SQLite.
 * @param reg Puntero al registro donde se cargarán los usuarios.
 * @param db Puntero a la base de datos.
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int cargarUsuariosDB(RegistroUsuarios* reg, sqlite3* db) {
    if (!reg || !db) return -1;
    const char* sql = "SELECT id, usuario, contrasena, rol FROM usuarios;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando SELECT: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    reg->cantidad = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Usuario user;
        user.id = sqlite3_column_int(stmt, 0);
        const unsigned char* us = sqlite3_column_text(stmt, 1);
        strncpy(user.usuario, (const char*)us, MAX_USUARIO);
        user.usuario[MAX_USUARIO - 1] = '\0';
        const unsigned char* pass = sqlite3_column_text(stmt, 2);
        strncpy(user.contrasena, (const char*)pass, MAX_CONTRASENA);
        user.contrasena[MAX_CONTRASENA - 1] = '\0';
        int rolInt = sqlite3_column_int(stmt, 3);
        user.rol = (rolInt == 1) ? ADMINISTRADOR : EMPLEADO;
        
        if (reg->cantidad == reg->capacidad) {
            int nuevaCapacidad = reg->capacidad * 2;
            Usuario* temp = realloc(reg->usuarios, nuevaCapacidad * sizeof(Usuario));
            if (!temp) {
                sqlite3_finalize(stmt);
                return -1;
            }
            reg->usuarios = temp;
            reg->capacidad = nuevaCapacidad;
        }
        reg->usuarios[reg->cantidad++] = user;
    }
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Registra un nuevo usuario en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña del usuario.
 * @param rol Rol del usuario (EMPLEADO o ADMINISTRADOR).
 * @return int Retorna 0 en caso de éxito o -1 en caso de error.
 */
int registrarUsuarioDB(sqlite3* db, const char* usuario, const char* contrasena, Rol rol) {
    if (!db || !usuario || !contrasena) return -1;
    const char* sql = "INSERT INTO usuarios (usuario, contrasena, rol) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando INSERT: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, (rol == ADMINISTRADOR) ? 1 : 0);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error insertando usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Autentica un usuario en la base de datos SQLite.
 * @param db Puntero a la base de datos.
 * @param usuario Nombre de usuario.
 * @param contrasena Contraseña ingresada.
 * @return Usuario* Retorna un puntero a un usuario autenticado (dinámicamente asignado) o NULL si la autenticación falla.
 */
Usuario* autenticarUsuarioDB(sqlite3* db, const char* usuario, const char* contrasena) {
    if (!db || !usuario || !contrasena) return NULL;
    const char* sql = "SELECT id, usuario, contrasena, rol FROM usuarios WHERE usuario = ? AND contrasena = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando SELECT para autenticar: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_STATIC);
    Usuario* user = NULL;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        user = malloc(sizeof(Usuario));
        if (user) {
            user->id = sqlite3_column_int(stmt, 0);
            const unsigned char* us = sqlite3_column_text(stmt, 1);
            strncpy(user->usuario, (const char*)us, MAX_USUARIO);
            user->usuario[MAX_USUARIO - 1] = '\0';
            const unsigned char* pass = sqlite3_column_text(stmt, 2);
            strncpy(user->contrasena, (const char*)pass, MAX_CONTRASENA);
            user->contrasena[MAX_CONTRASENA - 1] = '\0';
            int rolInt = sqlite3_column_int(stmt, 3);
            user->rol = (rolInt == 1) ? ADMINISTRADOR : EMPLEADO;
        }
    }
    sqlite3_finalize(stmt);
    return user;
}

/**
 * Menú interactivo para registrar un nuevo usuario utilizando la base de datos.
 * @param db Puntero a la base de datos.
 */
void menuRegistrarUsuarioDB(sqlite3* db) {
    char usuario[MAX_USUARIO + 1];
    char contrasena[MAX_CONTRASENA + 1];
    int rolInt;
    Rol rol;
    
    printf("\n\033[1;35m");
    printf("+----------------------------------+\n");
    printf("|        REGISTRAR USUARIO         |\n");
    printf("+----------------------------------+\n");
    printf("\033[0m");
    printf("Ingrese el nombre de usuario: ");
    scanf(" %[^\n]", usuario);
    printf("Ingrese la contrasenya: ");
    scanf(" %[^\n]", contrasena);
    printf("Seleccione el rol (0: Empleado, 1: Administrador): ");
    scanf("%d", &rolInt);
    rol = (rolInt == 1) ? ADMINISTRADOR : EMPLEADO;
    
    if (registrarUsuarioDB(db, usuario, contrasena, rol) == 0)
        printf("\n \033[1;32mUsuario registrado y guardado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al registrar el usuario.\033[0m\n");
}
