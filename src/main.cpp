// main.cpp
#include "client_net/admin_net.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <limits> 
#include <iomanip>

// Estructura para mostrar productos
struct Producto {
    int    id;
    std::string nombre;
    double precio;
    int    stock;
};

// Estructura para una transacción
struct Transaccion {
    int          id;
    std::string  tipo;
    std::string  fecha;
    double       total;
};

// Estructura para estadísticas
struct Stats {
    int     totalVentas;
    double  ingresos;
    double  promedio;
};

// Función auxiliar para centrar un texto en un campo de ancho `width`
std::string center(const std::string& s, int width) {
    int pad = width - (int)s.size();
    if (pad <= 0) return s;
    int padL = pad/2;
    int padR = pad - padL;
    return std::string(padL, ' ') + s + std::string(padR, ' ');
}

// Parsea  "OK|PRODUCTOS|id:nombre:precio:stock;..."
std::vector<Producto> parseProducts(const std::string& resp) {
    std::vector<Producto> lista;
    // saltar hasta el segundo '|'
    auto p1 = resp.find('|');
    if (p1 == std::string::npos) return lista;
    auto p2 = resp.find('|', p1+1);
    if (p2 == std::string::npos) return lista;
    std::string body = resp.substr(p2+1);
    std::istringstream ss(body);
    std::string item;
    while (std::getline(ss, item, ';')) {
        if (item.empty()) continue;
        std::istringstream is(item);
        std::string tok;
        Producto p;
        // id
        if (!std::getline(is, tok, ':')) continue;
        try { p.id = std::stoi(tok); } catch(...) { continue; }
        // nombre
        if (!std::getline(is, p.nombre, ':')) continue;
        // precio
        if (!std::getline(is, tok, ':')) continue;
        try { p.precio = std::stod(tok); } catch(...) { p.precio = 0; }
        // stock (resto)
        if (!std::getline(is, tok)) continue;
        try { p.stock = std::stoi(tok); } catch(...) { p.stock = 0; }
        lista.push_back(p);
    }
    return lista;
}

// Parsea la respuesta "OK|PRODUCTOS|id:nombre:precio:stock;..." a vector<Producto>
std::vector<Producto> parseProductList(const std::string& resp) {
    std::vector<Producto> lista;
    auto pos = resp.find("OK|PRODUCTOS|");
    if (pos == std::string::npos) return lista;
    std::string body = resp.substr(pos + 13);
    std::istringstream ss(body);
    std::string item;
    while (std::getline(ss, item, ';')) {
        std::istringstream is(item);
        std::string tok;
        Producto p;
        std::getline(is, tok, ':'); p.id = std::stoi(tok);
        std::getline(is, p.nombre, ':');
        std::getline(is, tok, ':'); p.precio = std::stod(tok);
        std::getline(is, tok, ':'); p.stock = std::stoi(tok);
        lista.push_back(p);
    }
    return lista;
}

// Parsea "OK|TRANSACTIONS|id:tipo:fecha:total;…"
std::vector<Transaccion> parseTrans(const std::string& resp) {
    std::vector<Transaccion> lista;
    // Saltar prefijo antes del segundo '|'
    size_t p1 = resp.find('|');
    if (p1 == std::string::npos) return lista;
    size_t p2 = resp.find('|', p1 + 1);
    if (p2 == std::string::npos) return lista;
    std::string body = resp.substr(p2 + 1);
    std::istringstream ss(body);
    std::string item;
    while (std::getline(ss, item, ';')) {
        if (item.empty()) continue;
        std::istringstream is(item);
        std::string tok;
        Transaccion t;
        // id
        if (!std::getline(is, tok, ':')) continue;
        try { t.id = std::stoi(tok); } catch(...) { continue; }
        // tipo
        if (!std::getline(is, t.tipo, ':')) continue;
        // fecha
        if (!std::getline(is, t.fecha, ':')) continue;
        // total
        if (!std::getline(is, tok)) continue;
        try { t.total = std::stod(tok); } catch(...) { t.total = 0; }
        lista.push_back(t);
    }
    return lista;
}

// Parser de la respuesta de estadísticas
Stats parseStats(const std::string& resp) {
    Stats st{0, 0.0, 0.0};
    // Buscamos la parte después del segundo '|'
    auto p1 = resp.find('|');
    if (p1 == std::string::npos) return st;
    auto p2 = resp.find('|', p1 + 1);
    if (p2 == std::string::npos) return st;
    std::string body = resp.substr(p2 + 1);

    std::istringstream ss(body);
    std::string pair;
    while (std::getline(ss, pair, ';')) {
        auto sep = pair.find(':');
        if (sep == std::string::npos) continue;
        std::string key = pair.substr(0, sep);
        std::string val = pair.substr(sep + 1);
        if (key == "TotalVentas") {
            st.totalVentas = std::stoi(val);
        } else if (key == "Ingresos") {
            st.ingresos = std::stod(val);
        } else if (key == "PromedioPorVenta") {
            st.promedio = std::stod(val);
        }
    }
    return st;
}

// Parsea la respuesta "OK|LOW_STOCK|id:stock;id:stock;..." a vector de pares
std::vector<std::pair<int,int>> parseLowStock(const std::string& resp) {
    std::vector<std::pair<int,int>> lista;
    auto pos = resp.find("OK|LOW_STOCK|");
    if (pos == std::string::npos) return lista;
    std::string body = resp.substr(pos + 13);
    std::istringstream ss(body);
    std::string item;
    while (std::getline(ss, item, ';')) {
        auto sep = item.find(':');
        if (sep == std::string::npos) continue;
        int id    = std::stoi(item.substr(0, sep));
        int stock = std::stoi(item.substr(sep + 1));
        lista.emplace_back(id, stock);
    }
    return lista;
}

// Muestra los productos en tabla
void showProducts(const std::vector<Producto>& v) {
    // Cabecera
    std::cout << std::right 
              << std::setw(4) << "ID" 
              << "  " << std::left  << std::setw(15) << "Nombre" 
              << std::right << std::setw(8) << "Precio" 
              << std::setw(8) << "Stock" 
              << "\n";

    std::cout << "----  " 
              << "---------------" 
              << "--------" 
              << "--------\n";

    for (auto& p : v) {
        std::cout 
            // ID a la derecha en 4 espacios
            << std::right << std::setw(4) << p.id 
            << "  "
            // Nombre a la izquierda en 15 espacios
            << std::left  << std::setw(15) << p.nombre 
            // Precio y stock a la derecha
            << std::right << std::setw(8) << p.precio 
            << std::setw(8) << p.stock 
            << "\n";
    }
}

// Muestra las transacciones en tabla
void showTransactions(const std::vector<Transaccion>& v) {
    // Cabecera
    std::cout 
      << std::right << std::setw(6)  << "ID"
      << "  "
      << std::left  << std::setw(10) << "Tipo"
      << std::right << std::setw(15) << "Fecha"
      << std::setw(10) << "Precio"
      << "\n";

    std::cout 
      << "------  ----------  ---------------  ----------\n";

    // Filas
    for (auto& t : v) {
        std::cout
          << std::right << std::setw(6)  << t.id
          << "  "
          << std::left  << std::setw(10) << t.tipo
          << std::right << std::setw(15) << t.fecha
          << std::setw(10) << t.total
          << "\n";
    }
}

// Muestra las estadísticas de ventas
void showStatsCentered(const Stats& s) {
    const int W = 20;  // ancho de cada columna

    // Encabezados centrados
    std::cout
      << center("Total Ventas",    W)
      << center("Ingresos",         W)
      << center("Promedio/Venta",   W)
      << "\n";

    // Línea separadora
    std::cout
      << std::string(W, '-') 
      << std::string(W, '-') 
      << std::string(W, '-') 
      << "\n";

    // Valores convertidos a string y centrados
    std::cout
      << center(std::to_string(s.totalVentas), W)
      << center(std::to_string((int)s.ingresos), W)
      << center(std::to_string((int)s.promedio),   W)
      << "\n";
}

// Muestra la tabla de low stock
void showLowStock(const std::vector<std::pair<int,int>>& v) {
    std::cout << "\nID\tStock\n";
    std::cout << "--------------\n";
    for (auto& p : v) {
        std::cout << std::setw(2) << p.first << "\t"
                  << std::setw(5) << p.second << "\n";
    }
}

int main() {
    int opcion = -1;
    do {
        std::cout << "\n--- MENU CLIENTE HITO 3 ---\n"
                  << "1) Listar productos activos\n"
                  << "2) Obtener producto\n"
                  << "3) Agregar producto\n"
                  << "4) Actualizar stock\n"
                  << "5) Eliminar producto\n"
                  << "6) Registrar venta\n"
                  << "7) Listar transacciones\n"
                  << "8) Estadisticas ventas\n"
                  << "9) Listar low stock\n"
                  << "0) Salir\n"
                  << "Opcion: ";
        std::cin >> opcion;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (opcion == 0) break;

        // Cada comando abre su propia conexión
        AdminNet net("127.0.0.1", 5000);
        if (!net.connectServer()) {
            std::cerr << "No se pudo conectar al servidor\n";
            continue;
        }

        std::string cmd, resp;
        switch (opcion) {
            case 1: {
                auto resp = net.sendCommand("LIST_PRODUCTS");
                auto prods = parseProducts(resp);
                if (prods.empty()) {
                    std::cout << "No hay productos activos.\n";
                } else {
                    showProducts(prods);
                }
                break;
            }
            case 2: {
                int id;
                std::cout << "ID: "; std::cin >> id;
                cmd = "GET_PRODUCT|" + std::to_string(id);
                resp = net.sendCommand(cmd);
                if (resp.rfind("OK|PRODUCT|", 0) == 0) {
                    // reutilizar parseProductList para formatear
                    auto v = parseProductList("OK|PRODUCTOS|" + resp.substr(11));
                    if (!v.empty()) showProducts(v);
                } else {
                    std::cout << "Producto no encontrado\n";
                }
                break;
            }

            case 3: {
                Producto p;
                std::cout << "ID: "; std::cin >> p.id;
                std::cin.ignore();
                std::cout << "Nombre: "; std::getline(std::cin, p.nombre);
                std::cout << "Precio: "; std::cin >> p.precio;
                std::cout << "Stock: ";  std::cin >> p.stock;
                std::ostringstream os;
                os << "ADD_PRODUCT|" 
                   << p.id << "|" << p.nombre << "|" 
                   << p.precio << "|" << p.stock;
                resp = net.sendCommand(os.str());
                std::cout << (resp == "OK|ADD_PRODUCT" ? "Añadido\n" : "Error\n");
                break;
            }

            case 4: {
                int id, delta;
                std::cout << "ID: "; std::cin >> id;
                std::cout << "Delta stock: "; std::cin >> delta;
                cmd = "UPDATE_STOCK|" + std::to_string(id) + "|" + std::to_string(delta);
                resp = net.sendCommand(cmd);
                std::cout << (resp == "OK|UPDATE_STOCK" ? "Actualizado\n" : "Error\n");
                break;
            }

            case 5: {
                int id;
                std::cout << "ID: "; std::cin >> id;
                cmd = "DELETE_PRODUCT|" + std::to_string(id);
                resp = net.sendCommand(cmd);
                std::cout << (resp == "OK|DELETE_PRODUCT" ? "Eliminado\n" : "Error\n");
                break;
            }

            case 6: {
                int id, qty;
                std::string date;
                std::cout << "ID producto: "; std::cin >> id;
                std::cout << "Cantidad: ";   std::cin >> qty;
                std::cin.ignore();
                std::cout << "Fecha (YYYY-MM-DD): "; std::getline(std::cin, date);
                std::ostringstream os;
                os << "RECORD_SALE|" 
                   << id << "|" << qty << "|" << date;
                resp = net.sendCommand(os.str());
                std::cout << (resp == "OK|RECORD_SALE" ? "Venta registrada\n" : "Error\n");
                break;
            }

            case 7: {
                auto resp = net.sendCommand("LIST_TRANSACTIONS");
                auto trs  = parseTrans(resp);
                if (trs.empty()) {
                    std::cout << "No hay transacciones registradas.\n";
                } else {
                    showTransactions(trs);
                }
                break;
            }

            case 8: {
                std::string start, end;
                std::cout << "Fecha inicio: "; std::getline(std::cin, start);
                std::cout << "Fecha fin   : "; std::getline(std::cin, end);
                std::string cmd = "GET_STATS_SALES|" + start + "|" + end;
                auto resp = net.sendCommand(cmd);
                // resp == "OK|STATS_SALES|TotalVentas:5;Ingresos:350;PromedioPorVenta:70"
                Stats st = parseStats(resp);
                showStatsCentered(st);
                break;
            }

            case 9: {
                int umbral;
                std::cout << "Umbral stock: "; 
                std::cin >> umbral;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string cmd = "LIST_LOW_STOCK|" + std::to_string(umbral);
                std::string resp = net.sendCommand(cmd);
                auto lows = parseLowStock(resp);
                showLowStock(lows);
                break;
            }

            default:
                std::cout << "Opción inválida\n";
        }

        net.closeConn();

    } while (true);

    std::cout << "Saliendo...\n";
    return 0;
}
