# Gestor de Inventarios para Pequeños Negocios

Este proyecto es un sistema de gestión de inventarios diseñado para pequeñas tiendas. Permite registrar y administrar productos, gestionar transacciones (ventas y compras) y controlar el acceso mediante un sistema de usuarios con roles (administrador y empleado).

---

## Tabla de Contenidos

- [Características](#características)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Compilación y Ejecución](#compilación-y-ejecución)
- [Uso de la Aplicación](#uso-de-la-aplicación)
  - [Login y Roles](#login-y-roles)
  - [Gestión de Inventario](#gestión-de-inventario)
  - [Gestión de Transacciones](#gestión-de-transacciones)
  - [Administración Avanzada](#administración-avanzada)
- [Persistencia de Datos](#persistencia-de-datos)
- [Autores](#autores)

---

## Características

- **Gestión de Inventario Dinámico:**  
  Permite cargar, visualizar, agregar, modificar y actualizar el stock de productos utilizando memoria dinámica y persistencia en fichero.

- **Gestión de Transacciones:**  
  Registra transacciones de ventas y compras, permitiendo incluir múltiples productos en cada operación. Los datos se guardan en un fichero para mantener la persistencia.

- **Sistema de Usuarios y Login:**  
  Implementa el registro y autenticación de usuarios. Los roles (administrador y empleado) determinan las funcionalidades disponibles.

- **Módulo de Administración:**  
  Un stub para funcionalidades avanzadas (informes, estadísticas, etc.) que se integrará en futuras fases.

- **Interfaz de Línea de Comandos:**  
  Menús interactivos y decorados con colores para ofrecer una experiencia visualmente consistente y atractiva.

---

## Estructura del Proyecto

La organización de archivos es la siguiente:

```plaintext
GestorInventario/ 
├── bbdd/ 
│ ├── db_init/ 
│ │ ├── db_init.c # Inicialización y configuración de la base de datos
│ │ └── db_init.h # Definición de funciones para la inicialización de la base de datos
│ ├── sqlite/ 
│ │ ├── shell.c # Código fuente de la interfaz de línea de comandos de SQLite
│ │ ├── sqlite3.c # Código fuente de SQLite 
│ │ ├── sqlite3.h # Definición de la API de SQLite
│ │ └── sqlite3ext.h # Extensiones de SQLite
├── src/ 
│ ├── admin_cliente/ 
│ │ ├── admin_cliente.c # Módulo stub para funcionalidades de administración avanzada 
│ │ └── admin_cliente.h # Definición de funciones para el módulo de administración
│ ├── colors/ 
│ │ └── colors.h # Definición de macros de colores para la interfaz 
│ ├── inventario/ 
│ │ ├── inventario.c # Implementación de la lógica de inventario 
│ │ └── inventario.h # Definición de funciones para el manejo del inventario
│ ├── login/ 
│ │ ├── login.c # Lógica de autenticación de usuarios 
│ │ └── login.h # Definición de funciones para el manejo del login
│ ├── transacciones/ 
│ │ ├── transacciones.c # Implementación de la lógica de transacciones (ventas/compras) 
│ │ └── transacciones.h # Definición de funciones para el manejo de transacciones
│ ├── usuarios/ 
│ │ ├── usuarios.c # Manejo de registro de usuarios y roles 
│ │ └── usuarios.h # Definición de funciones para el manejo de usuarios
│ └── main.c # Punto de entrada de la aplicación
├── main.exe # Ejecutable generado tras la compilación (en Windows) 
├── README.md # Documentación y guía de usuario 
└── inventario.sqlite # Archivo de la base de datos SQLite
```

---

## Compilación y Ejecución

### Compilación Manual

Si deseas compilar desde la terminal, puedes usar un comando similar a:

```bash
 gcc -g -I. -I./bbdd -I./src src/main.c src/admin_cliente/admin_cliente.c src/inventario/inventario.c src/login/login.c src/transacciones/transacciones.c src/usuarios/usuarios.c bbdd/db_init/db_init.c bbdd/sqlite/sqlite3.c -o main.exe
```

### Ejecución Manual

```bash
.\main.exe
```

---

## Uso de la Aplicación

### Login y Roles

#### Pantalla de Inicio

Al ejecutar el programa se muestra un menú principal donde puedes:

- Iniciar sesión (Login).
- Registrar un nuevo usuario.
- Salir de la aplicación.

#### Proceso de Login

- Se solicitarán las credenciales (nombre de usuario y contraseña) en un menú decorado.
- Si la autenticación es exitosa:
  - **Administrador:** Accede a un menú con opciones avanzadas, que incluyen la gestión completa del inventario, transacciones, y la posibilidad de registrar nuevos usuarios.
  - **Empleado:** Accede a un menú con opciones básicas, como listar productos, actualizar stock y crear transacciones.

---

### Gestión de Inventario

Dentro de los menús (tanto para administradores como para empleados) se pueden realizar las siguientes operaciones:

- **Listar Productos:**  
  Visualiza el inventario en una pantalla formateada con encabezados decorados y colores, incluyendo si el producto está activo o inactivo.

- **Agregar Producto:**  
  Mediante un menú interactivo se solicita al usuario:
  - Nombre del producto.
  - Precio.
  - Stock inicial.
  - Estado (activo/inactivo).

- **Actualizar Stock:**  
  Permite incrementar o decrementar el stock de un producto ingresando el ID del producto y la cantidad  
  (positivo para sumar, negativo para restar).

- **Modificar Producto:**  
  Ofrece un submenú donde se pueden modificar los atributos de un producto:
  - Cambiar precio.
  - Cambiar stock.
  - Cambiar nombre.
  - Marcar como inactivo (baja).

---

### Gestión de Transacciones

El sistema permite registrar transacciones que involucran uno o varios productos:

- **Crear Transacción:**  
  El usuario selecciona el tipo de transacción (Venta o Compra), ingresa la fecha y, a través de un menú interactivo, agrega uno o más ítems.  
  Para cada ítem, se solicita:
  - El ID del producto (se busca en el inventario).
  - La cantidad a transaccionar.

Solo se pueden agregar a la transacción productos que estén activos. Si se intenta agregar un producto inactivo, se muestra un mensaje de error.
Cada ítem se procesa automáticamente (calculando el total del ítem como el producto del precio del producto y la cantidad).  
Al finalizar, se calcula el total de la transacción y se guarda en el fichero de ventas.

---

### Persistencia de Datos

El sistema utiliza **SQLite** para mantener la persistencia de los datos, en lugar de utilizar ficheros de texto. La base de datos se crea (o actualiza) automáticamente al ejecutar la aplicación y se almacena en un único archivo (por ejemplo, `inventario.sqlite`). Las principales tablas de la base de datos son:

- **productos:**  
  Contiene la lista de productos, con campos para el identificador, nombre, precio, stock y estado (activo/inactivo).

- **transacciones:**  
  Registra las transacciones realizadas (ventas o compras), incluyendo el tipo, la fecha y el total de la transacción.

- **items_transaccion:**  
  Almacena los ítems asociados a cada transacción, indicando el producto, la cantidad, el precio unitario y el total por ítem.  
  (Esta tabla se relaciona con la tabla de transacciones a través de una clave foránea).

- **usuarios:**  
  Guarda los datos de los usuarios registrados, incluyendo el nombre de usuario, la contraseña y el rol (administrador o empleado).

---

### Administración Avanzada (se integrarán en fases posteriores)

El módulo de administración (admin_cliente) es un stub que muestra un menú interactivo con opciones como:

- Mostrar informes.
- Consultar estadísticas.
- Otras opciones futuras.

## Autores

- Jon López Carrillo
- Roberto Fernandez Barrios
