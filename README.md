# Gestor de Inventarios para Pequeños Negocios

Un sistema de gestión de inventarios y transacciones pensado para tiendas pequeñas.  
**Hito 2** está basado en una aplicación monolítica de consola con ficheros;  
**Hito 3** introduce un modelo cliente-servidor usando SQLite y sockets TCP.

---

## 📑 Contenido

- [Características](#-características)  
- [Estructura del proyecto](#-estructura-del-proyecto)  
- [Compilación y ejecución](#-compilación-y-ejecución)  
  - [Hito 2 (Monolítico)](#hito-2-monolítico)  
  - [Hito 3 (Cliente-servidor)](#hito-3-cliente-servidor)  
- [Uso de la aplicación](#-uso-de-la-aplicación)  
  - [Credenciales y roles](#credenciales-y-roles)  
  - [Menú administrador (Hito 2)](#menú-administrador-hito-2)  
  - [Menú cliente/empleado (Hito 3)](#menú-clienteempleado-hito-3)  
- [Persistencia de datos](#-persistencia-de-datos)  
- [Autores](#-autores)

---

## 🛠 Características

- **Hito 2 (Monolítico):**  
  - Gestión de inventario en memoria dinámica + ficheros de texto.  
  - Usuarios con roles y login en consola.  
  - Transacciones (ventas/compras) multi-ítem.

- **Hito 3 (Cliente-Servidor):**  
  - API TCP: comandos como `LIST_PRODUCTS`, `RECORD_SALE`, etc.  
  - Servidor en C++ con SQLite (amalgama compilada como objeto).  
  - Cliente C++ con menús formateados y colores.  
  - Persistencia y consultas SQL: productos activos, ventas, estadísticas, low-stock.

- **UI de Consola Mejorada:**  
  - Tablas con encabezados alineados.  
  - Colores configurable en `src/colors/colors.h`.  
  - Menús centrados y decorados.

---

## 📂 Estructura del Proyecto

```

/
├─ bbdd/
│   ├─ sqlite/            ← SQLite amalgamada
│   └─ db\_init/           ← Creación de tablas (Hito 2)
├─ src/
│   ├─ admin\_cliente/     ← Stub administrador (Hito 2)
│   ├─ client\_net/        ← Cliente TCP (Hito 3)
│   ├─ colors/            ← Definiciones ANSI para colores
│   ├─ inventario/        ← Módulo inventario (Hito 2)
│   ├─ login/             ← Módulo login (Hito 2)
│   ├─ server/            ← Servidor TCP + DBHandler (Hito 3)
│   ├─ transacciones/     ← Módulo transacciones (Hito 2)
│   ├─ usuarios/          ← Módulo usuarios (Hito 2)
│   ├─ main.c             ← Entrada Hito 2 (monolítico)
│   └─ main.cpp           ← Entrada Hito 3 (cliente TCP)
├─ Makefile               ← Reglas para Hito 2 y Hito 3
├─ .gitignore
└─ README.md

````

---

## ⚙️ Compilación y Ejecución

### Hito 2 (Monolítico)

```bash
make legacy
./main.exe
````

O compilación manual:

```bash
gcc -g \
  -I. -I./bbdd -I./src \
  src/main.c src/admin_cliente/admin_cliente.c \
  src/inventario/inventario.c src/login/login.c \
  src/transacciones/transacciones.c src/usuarios/usuarios.c \
  bbdd/db_init/db_init.c bbdd/sqlite/sqlite3.c \
  -o main.exe
```

### Hito 3 (Cliente-Servidor)

1. **Compila todo**

   ```bash
   make
   ```
2. **Arranca servidor**

   ```bash
   ./servidor.exe inventario.sqlite 5000
   ```
3. **En otra consola, lanza el cliente**

   ```bash
   ./cliente.exe
   ```

---

## 🚀 Uso de la Aplicación

### Credenciales y Roles

* **Administrador:**
  Tiene acceso a registrar usuarios, ver estadísticas y CRUD completo.

* **Empleado:**
  Solo lista productos, actualiza stock y registra ventas.

Las credenciales se almacenan en la tabla `usuarios` de SQLite.

---

### Menú Administrador (Hito 2)

```text
+-------------------------------+
|    SISTEMA GESTOR INVENTARIO  |
+-------------------------------+
| 1. Listar productos           |
| 2. Agregar/modificar producto |
| 3. Eliminar producto          |
| 4. Crear transaccion          |
| 5. Registrar usuario          |
| 6. Salir                      |
+-------------------------------+
```

Opciones basadas en ficheros de texto y módulos en C.

---

### Menú Cliente/Empleado (Hito 3)

```text
--- MENU CLIENTE HITO 3 ---
 1) Listar productos activos
 2) Obtener producto
 3) Agregar producto
 4) Actualizar stock
 5) Eliminar producto
 6) Registrar venta
 7) Listar transacciones
 8) Estadisticas ventas
 9) Listar low stock
 0) Salir
```

Cada selección abre su propia conexión TCP, envía un comando y muestra la respuesta en tablas formateadas y colores.

---

## 💾 Persistencia de Datos

Usamos un único fichero SQLite (`inventario.sqlite`) con estas tablas:

* **productos:** `id, nombre, precio, stock, activo`
* **usuarios:**  `id, usuario, contrasena, rol`
* **transacciones:** `id, tipo, fecha, total`
* **items\_transaccion:**
  `id, transaccion_id → transacciones.id, producto_id → productos.id, cantidad, precio_unitario, total_item`

La base de datos se crea/actualiza al iniciar el servidor.

---

## 🖋 Autores

* Jon López Carrillo
* Roberto Fernández Barrios
