# Gestor de Inventarios para Pequeños Negocios

Este proyecto es un sistema de gestión de inventarios diseñado para pequeñas tiendas. Permite registrar y administrar productos, gestionar transacciones (ventas y compras) y controlar el acceso mediante un sistema de usuarios con roles (administrador y empleado).

---

## Tabla de Contenidos

- [Características](#características)
- [Compilación y Ejecución](#compilación-y-ejecución)
- [Uso de la Aplicación](#uso-de-la-aplicación)
  - [Login y Roles](#login-y-roles)
  - [Gestión de Inventario](#gestión-de-inventario)
  - [Gestión de Transacciones](#gestión-de-transacciones)
  - [Administración Avanzada](#administración-avanzada)

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

## Gestión de Inventario

Dentro de los menús (tanto para administradores como para empleados) se pueden realizar las siguientes operaciones:

- **Listar Productos:**  
  Visualiza el inventario activo en una pantalla formateada con encabezados decorados y colores.

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

## Gestión de Transacciones

El sistema permite registrar transacciones que involucran uno o varios productos:

- **Crear Transacción:**  
  El usuario selecciona el tipo de transacción (Venta o Compra), ingresa la fecha y, a través de un menú interactivo, agrega uno o más ítems.  
  Para cada ítem, se solicita:
  - El ID del producto (se busca en el inventario).
  - La cantidad a transaccionar.

Cada ítem se procesa automáticamente (calculando el total del ítem como el producto del precio del producto y la cantidad).  
Al finalizar, se calcula el total de la transacción y se guarda en el fichero de ventas.

---

Aquí tienes un ejemplo actualizado para el apartado "Persistencia de Datos" en el README:

---

**Persistencia de Datos**

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

## Administración Avanzada (se integrarán en fases posteriores)

El módulo de administración (admin_cliente) es un stub que muestra un menú interactivo con opciones como:

- Mostrar informes.
- Consultar estadísticas.
- Otras opciones futuras.

## Autores

- Jon López Carrillo
- Roberto Fernandez Barrios
