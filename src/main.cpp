// main.cpp
#include "client_net/admin_net.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <limits> 

// Estructura para mostrar productos
struct Producto {
    int    id;
    std::string nombre;
    double precio;
    int    stock;
};

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

void showProducts(const std::vector<Producto>& v) {
    std::cout << "\nID\tNombre\t\tPrecio\tStock\n";
    std::cout << "-------------------------------------\n";
    for (const auto& p : v) {
        std::cout << p.id << "\t" 
                  << p.nombre << "\t\t" 
                  << p.precio << "\t" 
                  << p.stock << "\n";
    }
}

int main() {
    int opcion = -1;
    do {
        std::cout << "\n--- MENU CLIENTE HITO 3 ---\n"
                  << "1) Listar productos\n"
                  << "2) Obtener producto\n"
                  << "3) Agregar producto\n"
                  << "4) Actualizar stock\n"
                  << "5) Eliminar producto\n"
                  << "6) Registrar venta\n"
                  << "7) Listar ventas\n"
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
            case 1:
                resp = net.sendCommand("LIST_PRODUCTS");
                showProducts(parseProductList(resp));
                break;

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
                std::string start, end;
                std::cout << "Fecha inicio: "; std::getline(std::cin, start);
                std::cout << "Fecha fin   : "; std::getline(std::cin, end);
                cmd = "LIST_SALES|" + start + "|" + end;
                resp = net.sendCommand(cmd);
                std::cout << resp << "\n";
                break;
            }

            case 8: {
                std::string start, end;
                std::cout << "Fecha inicio: "; std::getline(std::cin, start);
                std::cout << "Fecha fin   : "; std::getline(std::cin, end);
                cmd = "GET_STATS_SALES|" + start + "|" + end;
                resp = net.sendCommand(cmd);
                std::cout << resp << "\n";
                break;
            }

            case 9: {
                int umbral;
                std::cout << "Umbral stock: "; std::cin >> umbral;
                cmd = "LIST_LOW_STOCK|" + std::to_string(umbral);
                resp = net.sendCommand(cmd);
                std::cout << resp << "\n";
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
