# Makefile para compilar en Windows (MinGW) usando SQLite amalgamada como objeto C

CXX        = g++
CC         = gcc

# Flags para C++
CPPFLAGS   = -std=c++17 -I./src -I./bbdd/sqlite -I./bbdd/db_init -Wall -Wextra
# Flags para C
CFLAGS     = -I./bbdd/sqlite -I./bbdd/db_init -DSQLITE_THREADSAFE=0

# Librerías para enlazar
LDFLAGS    = -lws2_32      # Winsock

# Fuentes de servidor (C++)
SERVER_SRCS = src/server/server.cpp \
              src/server/db_handler.cpp

# Objeto de SQLite compilado como C
SQLITE_OBJ  = sqlite3.o

# Fuentes de cliente Hito 3 (solo C++)
CLIENT_CPP_SRCS = src/client_net/admin_net.cpp \
                  src/main.cpp

# Objetos intermedios cliente
CLIENT_OBJS    = admin_net.o main.o

# Flags para el legacy (Hito 2)
LEGACY_CFLAGS = -I./src -I./bbdd -I./bbdd/db_init -I./bbdd/sqlite
# Fuentes de la versión monolítica antigua (Hito 2)
LEGACY_SRCS = bbdd/db_init/db_init.c \
              bbdd/sqlite/sqlite3.c \
              src/main.c \
              src/admin_cliente/admin_cliente.c \
              src/inventario/inventario.c \
              src/login/login.c \
              src/transacciones/transacciones.c \
              src/usuarios/usuarios.c

# Binarios de salida
SERVER_BIN  = servidor.exe
CLIENT_BIN  = cliente.exe
LEGACY_BIN  = main.exe

.PHONY: all clean

all: $(SERVER_BIN) $(CLIENT_BIN) $(LEGACY_BIN)

# Paso 1: compilar sqlite3.c como C
$(SQLITE_OBJ): bbdd/sqlite/sqlite3.c
	$(CC) $(CFLAGS) -c $< -o $@

# Paso 2: compilar y enlazar el servidor (C++)
$(SERVER_BIN): $(SERVER_SRCS) $(SQLITE_OBJ)
	$(CXX) $(CPPFLAGS) $(SERVER_SRCS) $(SQLITE_OBJ) $(LDFLAGS) -o $@

# Paso 3: compilar objetos de cliente
admin_net.o: src/client_net/admin_net.cpp src/client_net/admin_net.h
	$(CXX) -c $(CPPFLAGS) $< -o $@

main.o: src/main.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

# Paso 4: enlazar cliente
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CXX) $(CPPFLAGS) $(CLIENT_OBJS) $(LDFLAGS) -o $@

# Regla para generar el ejecutable monolítico (Hito 2)
$(LEGACY_BIN): $(LEGACY_SRCS)
	$(CC) -g $(LEGACY_CFLAGS) \
	      bbdd/db_init/db_init.c bbdd/sqlite/sqlite3.c \
	      src/main.c \
	      src/admin_cliente/admin_cliente.c \
	      src/inventario/inventario.c \
	      src/login/login.c \
	      src/transacciones/transacciones.c \
	      src/usuarios/usuarios.c \
	      -o $(LEGACY_BIN)

clean:
	-@del /Q $(SERVER_BIN)     2>nul
	-@del /Q $(CLIENT_BIN)     2>nul
	-@del /Q $(LEGACY_BIN)     2>nul
	-@del /Q $(SQLITE_OBJ)     2>nul
	-@del /Q admin_net.o       2>nul
	-@del /Q main.o            2>nul