// src/server/db_handler.h
#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <string>
#include <vector>
#include <experimental/optional>
#include <sqlite3.h>

namespace stdx = std::experimental;

// Estructura para productos
struct Producto {
    int         id;
    std::string nombre;
    double      precio;
    int         stock;
};

// Estructura para una venta individual
struct Transaccion {
    int          id;
    std::string  tipo;
    std::string  fecha;
    double       total;
};

// Estructura con estadísticas de ventas
struct SalesStats {
    int    totalCount;
    double totalRevenue;
    double avgPerSale;
};

class DBHandler {
public:
    explicit DBHandler(const std::string& db_path);
    ~DBHandler();

    // Abre / cierra la conexión a la base de datos SQLite
    bool open();
    void close();

    // Productos
    std::vector<Producto>             listProducts();
    bool                              addProduct(const Producto& p);
    bool                              updateStock(int id, int delta);
    stdx::optional<Producto>         getProduct(int id);
    bool                              deleteProduct(int id);
    std::vector<Producto>             listLowStock(int threshold);

    // Ventas
    bool                              recordSale(int productId, int qty, const std::string& date);
    std::vector<Transaccion>          listTransactions();
    SalesStats                        getSalesStats(const std::string& start, const std::string& end);

private:
    std::string db_path_;
    sqlite3*    db_;
};

#endif // DB_HANDLER_H
