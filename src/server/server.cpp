// server.cpp
// Versión solo Windows, sin lógica condicional

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "server.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <experimental/optional>
namespace stdx = std::experimental;

Server::Server(const std::string& db_path, uint16_t port)
    : db_path_(db_path), port_(port), listen_sock_(INVALID_SOCKET), db_(db_path) {}

Server::~Server() {
    if (listen_sock_ != INVALID_SOCKET) {
        closesocket(listen_sock_);
    }
}

bool Server::init() {
    // Inicializar Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup falló\n";
        return false;
    }

    // Abrir base de datos
    if (!db_.open()) {
        std::cerr << "Error: no se pudo abrir la BD en " << db_path_ << "\n";
        return false;
    }

    // Crear socket
    listen_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock_ == INVALID_SOCKET) {
        std::cerr << "Error: socket() falló, código " << WSAGetLastError() << "\n";
        return false;
    }

    // Reusar dirección
    BOOL opt = TRUE;
    setsockopt(listen_sock_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    // Bind
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);
    if (bind(listen_sock_, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind() falló, código " << WSAGetLastError() << "\n";
        return false;
    }

    // Listen
    if (listen(listen_sock_, 10) == SOCKET_ERROR) {
        std::cerr << "listen() falló, código " << WSAGetLastError() << "\n";
        return false;
    }

    std::cout << "Servidor escuchando en puerto " << port_ << "\n";
    return true;
}

void Server::run() {
    for (;;) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        SOCKET client_sock = accept(listen_sock_, reinterpret_cast<SOCKADDR*>(&client_addr), &client_len);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "accept() falló, código " << WSAGetLastError() << "\n";
            continue;
        }

        // Leer comando
        char buffer[2048];
        int received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (received == SOCKET_ERROR || received == 0) {
            closesocket(client_sock);
            continue;
        }
        buffer[received] = '\0';
        std::string cmd(buffer);

        // Procesar y responder
        std::string resp = handleCommand(cmd);
        send(client_sock, resp.c_str(), static_cast<int>(resp.size()), 0);
        closesocket(client_sock);
    }
}

std::string Server::handleCommand(const std::string& cmd) {
    std::istringstream ss(cmd);
    std::string action;
    std::getline(ss, action, '|');

    if (action == "LIST_PRODUCTS") {
        auto productos = db_.listProducts();
        std::ostringstream out;
        out << "OK|PRODUCTOS|";
        for (size_t i = 0; i < productos.size(); ++i) {
            const auto& p = productos[i];
            out << p.id << ":" << p.nombre << ":" << p.precio << ":" << p.stock;
            if (i + 1 < productos.size()) out << ";";
        }
        return out.str();
    }
    else if (action == "ADD_PRODUCT") {
        Producto p;
        std::string token;
        std::getline(ss, token, '|'); p.id = std::stoi(token);
        std::getline(ss, p.nombre, '|');
        std::getline(ss, token, '|'); p.precio = std::stod(token);
        std::getline(ss, token, '|'); p.stock = std::stoi(token);
        return db_.addProduct(p) ? "OK|ADD_PRODUCT" : "ERROR|ADD_PRODUCT";
    }
    else if (action == "UPDATE_STOCK") {
        int id, delta;
        std::string token;
        std::getline(ss, token, '|'); id = std::stoi(token);
        std::getline(ss, token, '|'); delta = std::stoi(token);
        return db_.updateStock(id, delta) ? "OK|UPDATE_STOCK" : "ERROR|UPDATE_STOCK";
    }
    else if (action == "GET_PRODUCT") {
        std::string token;
        std::getline(ss, token, '|');
        int id = std::stoi(token);

        stdx::optional<Producto> optP = db_.getProduct(id);
        if (optP) {
            const Producto& p = *optP;
            std::ostringstream out;
            out << "OK|PRODUCT|"
                << p.id << ":" << p.nombre << ":" << p.precio << ":" << p.stock;
            return out.str();
        }
        return "ERROR|GET_PRODUCT";
    }
    else if (action == "DELETE_PRODUCT") {
        std::string token;
        std::getline(ss, token, '|');
        int id = std::stoi(token);
        return db_.deleteProduct(id) ? "OK|DELETE_PRODUCT" : "ERROR|DELETE_PRODUCT";
    }
    else if (action == "RECORD_SALE") {
        std::string token, date;
        int id, qty;
        std::getline(ss, token, '|'); id  = std::stoi(token);
        std::getline(ss, token, '|'); qty = std::stoi(token);
        std::getline(ss, date,  '|');
        return db_.recordSale(id, qty, date) ? "OK|RECORD_SALE" : "ERROR|RECORD_SALE";
    }
    else if (action == "LIST_TRANSACTIONS") {
        auto trs = db_.listTransactions(); // Implementa este método en tu DBHandler
        std::ostringstream out;
        out << "OK|TRANSACTIONS|";
        for (size_t i = 0; i < trs.size(); ++i) {
            const auto& t = trs[i];
            out << t.id << ":" << t.tipo << ":" << t.fecha << ":" << t.total;
            if (i+1 < trs.size()) out << ";";
        }
        return out.str();
    }
    else if (action == "GET_STATS_SALES") {
        std::string start, end;
        std::getline(ss, start, '|');
        std::getline(ss, end,   '|');
        auto stats = db_.getSalesStats(start, end);
        std::ostringstream out;
        out << "OK|STATS_SALES|"
            << "TotalVentas:"      << stats.totalCount   << ";"
            << "Ingresos:"         << stats.totalRevenue << ";"
            << "PromedioPorVenta:" << stats.avgPerSale;
        return out.str();
    }
    else if (action == "LIST_LOW_STOCK") {
        std::string token;
        std::getline(ss, token, '|');
        int umbral = std::stoi(token);
        auto lows = db_.listLowStock(umbral);
        std::ostringstream out;
        out << "OK|LOW_STOCK|";
        for (size_t i = 0; i < lows.size(); ++i) {
            const auto& p = lows[i];
            out << p.id << ":" << p.stock;
            if (i + 1 < lows.size()) out << ";";
        }
        return out.str();
    }
    else {
        return "ERROR|UNKNOWN_COMMAND";
    }
}

int main(int argc, char* argv[]) {
    std::string dbfile = "inventario.sqlite";
    uint16_t port = 5000;
    if (argc >= 2) dbfile = argv[1];
    if (argc >= 3) port = static_cast<uint16_t>(std::stoi(argv[2]));

    Server srv(dbfile, port);
    if (!srv.init()) return 1;
    srv.run();

    WSACleanup();
    return 0;
}
