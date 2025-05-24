CXX        = g++
CC         = gcc

CPPFLAGS   = -std=c++17 -I./src -I./bbdd/sqlite -I./bbdd/db_init -Wall -Wextra
CFLAGS     = -I./bbdd/sqlite -I./bbdd/db_init -DSQLITE_THREADSAFE=0

LDFLAGS    = -lws2_32     

SERVER_SRCS = src/server/server.cpp \
              src/server/db_handler.cpp

SQLITE_OBJ  = sqlite3.o

CLIENT_CPP_SRCS = src/client_net/admin_net.cpp \
                  src/main.cpp

CLIENT_OBJS    = admin_net.o main.o

LEGACY_CFLAGS = -I./src -I./bbdd -I./bbdd/db_init -I./bbdd/sqlite

LEGACY_SRCS = bbdd/db_init/db_init.c \
              bbdd/sqlite/sqlite3.c \
              src/main.c \
              src/admin_cliente/admin_cliente.c \
              src/inventario/inventario.c \
              src/login/login.c \
              src/transacciones/transacciones.c \
              src/usuarios/usuarios.c

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
