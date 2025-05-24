#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <string>
#include <vector>
#include <experimental/optional>
#include <sqlite3.h>

namespace stdx = std::experimental;

struct Producto {
    int         id;
    std::string nombre;
    double      precio;
    int         stock;
};

struct Transaccion {
    int          id;
    std::string  tipo;
    std::string  fecha;
    double       total;
};

struct SalesStats {
    int    totalCount;
    double totalRevenue;
    double avgPerSale;
};

class DBHandler {
public:
    explicit DBHandler(const std::string& db_path);
    ~DBHandler();

    bool open();
    void close();

    std::vector<Producto>             listProducts();
    bool                              addProduct(const Producto& p);
    bool                              updateStock(int id, int delta);
    stdx::optional<Producto>         getProduct(int id);
    bool                              deleteProduct(int id);
    std::vector<Producto>             listLowStock(int threshold);

    bool                              recordSale(int productId, int qty, const std::string& date);
    std::vector<Transaccion>          listTransactions();
    SalesStats                        getSalesStats(const std::string& start, const std::string& end);

private:
    std::string db_path_;
    sqlite3*    db_;
};

#endif
