// main.cpp
#include "client_net/admin_net.h"
#include "colors/colors.h"
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

// Imprime el menú con colores
void printMenu() {
    // Titulo en cyan
    std::cout << COLOR_CYAN;
    std::cout << "====================================\n";
    std::cout << "   MENU CLIENTE HITO 3\n";
    std::cout << "====================================\n";
    std::cout << COLOR_RESET;
    // Opciones: numero en verde, texto en blanco
    std::cout << COLOR_GREEN << " 1) " << COLOR_RESET << "Listar productos activos\n";
    std::cout << COLOR_GREEN << " 2) " << COLOR_RESET << "Obtener producto\n";
    std::cout << COLOR_GREEN << " 3) " << COLOR_RESET << "Agregar producto\n";
    std::cout << COLOR_GREEN << " 4) " << COLOR_RESET << "Actualizar stock\n";
    std::cout << COLOR_GREEN << " 5) " << COLOR_RESET << "Eliminar producto\n";
    std::cout << COLOR_GREEN << " 6) " << COLOR_RESET << "Registrar venta\n";
    std::cout << COLOR_GREEN << " 7) " << COLOR_RESET << "Listar transacciones\n";
    std::cout << COLOR_GREEN << " 8) " << COLOR_RESET << "Estadisticas ventas\n";
    std::cout << COLOR_GREEN << " 9) " << COLOR_RESET << "Listar low stock\n";
    std::cout << COLOR_GREEN << " 0) " << COLOR_RESET << "Salir\n";
    std::cout << "Opcion: ";
}

int main() {
    int opcion = -1;
    do {
        printMenu();
        if(!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << COLOR_RED << "Entrada invalida\n" << COLOR_RESET;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (opcion == 0) break;

        AdminNet net("127.0.0.1", 5000);
        if (!net.connectServer()) {
            std::cout << COLOR_RED << "✖ No se pudo conectar al servidor\n" << COLOR_RESET;
            continue;
        }

        switch (opcion) {
            case 1: {
                auto resp = net.sendCommand("LIST_PRODUCTS");
                auto prods = parseProducts(resp);
                if (prods.empty()) {
                    std::cout << COLOR_YELLOW << "— No hay productos activos\n" << COLOR_RESET;
                } else {
                    showProducts(prods);
                }
                break;
            }
            case 2: {
                int id; 
                std::cout << COLOR_YELLOW << "ID: " << COLOR_RESET;
                std::cin >> id;
                auto resp = net.sendCommand("GET_PRODUCT|" + std::to_string(id));
                if (resp.rfind("OK|PRODUCT|", 0) == 0) {
                    auto v = parseProducts("OK|PRODUCTOS|" + resp.substr(11));
                    showProducts(v);
                } else {
                    std::cout << COLOR_RED << "✖ Producto no encontrado\n" << COLOR_RESET;
                }
                break;
            }
            case 3: {
                Producto p;
                std::cout << COLOR_YELLOW << "ID: "      << COLOR_RESET; std::cin >> p.id;
                std::cin.ignore();
                std::cout << COLOR_YELLOW << "Nombre: "  << COLOR_RESET; std::getline(std::cin, p.nombre);
                std::cout << COLOR_YELLOW << "Precio: "  << COLOR_RESET; std::cin >> p.precio;
                std::cout << COLOR_YELLOW << "Stock: "   << COLOR_RESET; std::cin >> p.stock;
                std::ostringstream os;
                os << "ADD_PRODUCT|" 
                   << p.id << "|" << p.nombre << "|" 
                   << p.precio << "|" << p.stock;
                auto resp = net.sendCommand(os.str());
                if (resp == "OK|ADD_PRODUCT")
                    std::cout << COLOR_GREEN << "✔ Producto añadido\n" << COLOR_RESET;
                else
                    std::cout << COLOR_RED   << "✖ Error al añadir\n" << COLOR_RESET;
                break;
            }
            case 4: {
                int id, delta;
                std::cout << COLOR_YELLOW << "ID: "          << COLOR_RESET; std::cin >> id;
                std::cout << COLOR_YELLOW << "Delta stock: "<< COLOR_RESET; std::cin >> delta;
                auto resp = net.sendCommand(
                    "UPDATE_STOCK|" + std::to_string(id) + "|" + std::to_string(delta));
                if (resp == "OK|UPDATE_STOCK")
                    std::cout << COLOR_GREEN << "✔ Stock actualizado\n" << COLOR_RESET;
                else
                    std::cout << COLOR_RED   << "✖ Error al actualizar\n" << COLOR_RESET;
                break;
            }
            case 5: {
                int id;
                std::cout << COLOR_YELLOW << "ID: " << COLOR_RESET; std::cin >> id;
                auto resp = net.sendCommand("DELETE_PRODUCT|" + std::to_string(id));
                if (resp == "OK|DELETE_PRODUCT")
                    std::cout << COLOR_GREEN << "✔ Producto eliminado\n" << COLOR_RESET;
                else
                    std::cout << COLOR_RED   << "✖ Error al eliminar\n" << COLOR_RESET;
                break;
            }
            case 6: {
                int id, qty; std::string date;
                std::cout << COLOR_YELLOW << "ID producto: " << COLOR_RESET; std::cin >> id;
                std::cout << COLOR_YELLOW << "Cantidad: "     << COLOR_RESET; std::cin >> qty;
                std::cin.ignore();
                std::cout << COLOR_YELLOW << "Fecha (YYYY-MM-DD): " << COLOR_RESET;
                std::getline(std::cin, date);
                std::ostringstream os;
                os << "RECORD_SALE|" << id << "|" << qty << "|" << date;
                auto resp = net.sendCommand(os.str());
                if (resp == "OK|RECORD_SALE")
                    std::cout << COLOR_GREEN << "✔ Venta registrada\n" << COLOR_RESET;
                else
                    std::cout << COLOR_RED   << "✖ Error al registrar\n" << COLOR_RESET;
                break;
            }
            case 7: {
                auto resp = net.sendCommand("LIST_TRANSACTIONS");
                auto trs  = parseTrans(resp);
                if (trs.empty())
                    std::cout << COLOR_YELLOW << "— No hay transacciones\n" << COLOR_RESET;
                else
                    showTransactions(trs);
                break;
            }
            case 8: {
                std::string start, end;
                std::cout << COLOR_YELLOW << "Fecha inicio: " << COLOR_RESET; std::getline(std::cin, start);
                std::cout << COLOR_YELLOW << "Fecha fin   : " << COLOR_RESET; std::getline(std::cin, end);
                auto resp = net.sendCommand("GET_STATS_SALES|" + start + "|" + end);
                auto st   = parseStats(resp);
                showStatsCentered(st);
                break;
            }
            case 9: {
                int umbral;
                std::cout << COLOR_YELLOW << "Umbral stock: " << COLOR_RESET;
                std::cin >> umbral;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                auto resp = net.sendCommand("LIST_LOW_STOCK|" + std::to_string(umbral));
                auto lows = parseLowStock(resp);
                showLowStock(lows);
                break;
            }
            default:
                std::cout << COLOR_RED << "✖ Opcióo invalida\n" << COLOR_RESET;
        }

        net.closeConn();
    } while (true);

    std::cout << COLOR_MAGENTA << "\n¡Hasta luego!\n" << COLOR_RESET;
    return 0;
}
