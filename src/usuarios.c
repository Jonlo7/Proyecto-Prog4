#include "usuarios.h"

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

int cargarUsuarios(RegistroUsuarios* reg) {
    if (!reg) return -1;
    FILE* file = fopen(USUARIOS_FILE, "r");
    if (!file) return -1;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '\r')
            continue;
        Usuario user;
        int rolInt;
        int items = sscanf(line, "%d|%30[^|]|%30[^|]|%d", 
                           &user.id, user.usuario, user.contrasena, &rolInt);
        if (items < 4)
            continue;
        user.rol = (rolInt == 1) ? ADMINISTRADOR : EMPLEADO;
        
        if (reg->cantidad == reg->capacidad) {
            int nuevaCapacidad = reg->capacidad * 2;
            Usuario* temp = realloc(reg->usuarios, nuevaCapacidad * sizeof(Usuario));
            if (!temp) {
                fclose(file);
                return -1;
            }
            reg->usuarios = temp;
            reg->capacidad = nuevaCapacidad;
        }
        reg->usuarios[reg->cantidad++] = user;
    }
    fclose(file);
    return 0;
}

int guardarUsuarios(const RegistroUsuarios* reg) {
    if (!reg) return -1;
    FILE* file = fopen(USUARIOS_FILE, "w");
    if (!file) return -1;
    
    for (int i = 0; i < reg->cantidad; i++) {
        const Usuario* user = &reg->usuarios[i];
        fprintf(file, "%d|%s|%s|%d\n", user->id, user->usuario, user->contrasena, 
                (user->rol == ADMINISTRADOR) ? 1 : 0);
    }
    fclose(file);
    return 0;
}

int registrarUsuario(RegistroUsuarios* reg, const char* usuario, const char* contrasena, Rol rol) {
    if (!reg || !usuario || !contrasena) return -1;
    
    int nextId = 0;
    for (int i = 0; i < reg->cantidad; i++) {
        if (reg->usuarios[i].id > nextId)
            nextId = reg->usuarios[i].id;
    }
    nextId++;
    
    if (reg->cantidad == reg->capacidad) {
        int nuevaCapacidad = reg->capacidad * 2;
        Usuario* temp = realloc(reg->usuarios, nuevaCapacidad * sizeof(Usuario));
        if (!temp) return -1;
        reg->usuarios = temp;
        reg->capacidad = nuevaCapacidad;
    }
    
    Usuario user;
    user.id = nextId;
    strncpy(user.usuario, usuario, MAX_USUARIO);
    user.usuario[MAX_USUARIO] = '\0';
    strncpy(user.contrasena, contrasena, MAX_CONTRASENA);
    user.contrasena[MAX_CONTRASENA] = '\0';
    user.rol = rol;
    
    reg->usuarios[reg->cantidad++] = user;
    return 0;
}

Usuario* autenticarUsuario(const RegistroUsuarios* reg, const char* usuario, const char* contrasena) {
    if (!reg || !usuario || !contrasena) return NULL;
    for (int i = 0; i < reg->cantidad; i++) {
        if (strcmp(reg->usuarios[i].usuario, usuario) == 0 &&
            strcmp(reg->usuarios[i].contrasena, contrasena) == 0) {
            return &reg->usuarios[i];
        }
    }
    return NULL;
}

void menuRegistrarUsuario(RegistroUsuarios* reg) {
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
    
    if (registrarUsuario(reg, usuario, contrasena, rol) == 0)
        printf("\n \033[1;32mUsuario registrado exitosamente.\033[0m\n");
    else
        printf("\n \033[1;31mError al registrar el usuario.\033[0m\n");
}
