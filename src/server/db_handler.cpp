// src/server/db_handler.cpp
#include "db_handler.h"
#include <sqlite3.h>
#include <iostream>


DBHandler::DBHandler(const std::string& db_path)
  : db_path_(db_path),
    db_(nullptr)
{}

DBHandler::~DBHandler() {
    close();
}

// Abre la base de datos (db_ es sqlite3*)
bool DBHandler::open() {
    if (sqlite3_open(db_path_.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Error abriendo BD: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    return true;
}

void DBHandler::close() {
    if (db_) sqlite3_close(db_);
}

// Lista todos los productos activos
std::vector<Producto> DBHandler::listProducts() {
    const char* sql = 
        "SELECT id, nombre, precio, stock "
        "FROM productos "
        "WHERE activo = 1;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Producto> salida;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return salida;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Producto p;
        p.id     = sqlite3_column_int(stmt, 0);
        p.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.precio = sqlite3_column_double(stmt, 2);
        p.stock  = sqlite3_column_int(stmt, 3);
        salida.push_back(p);
    }
    sqlite3_finalize(stmt);
    return salida;
}

// Inserta un producto (lo marca activo = 1)
bool DBHandler::addProduct(const Producto& p) {
    const char* sql =
        "INSERT INTO productos (id, nombre, precio, stock, activo) "
        "VALUES (?, ?, ?, ?, 1);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_bind_int   (stmt, 1, p.id);
    sqlite3_bind_text  (stmt, 2, p.nombre.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, p.precio);
    sqlite3_bind_int   (stmt, 4, p.stock);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// Ajusta stock (puede ser positivo o negativo)
bool DBHandler::updateStock(int id, int delta) {
    const char* sql =
        "UPDATE productos "
        "SET stock = stock + ? "
        "WHERE id = ? AND activo = 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_bind_int(stmt, 1, delta);
    sqlite3_bind_int(stmt, 2, id);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// Obtiene un producto concreto
stdx::optional<Producto> DBHandler::getProduct(int id) {
    const char* sql =
        "SELECT id, nombre, precio, stock "
        "FROM productos "
        "WHERE id = ? AND activo = 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return stdx::nullopt;
    }
    sqlite3_bind_int(stmt, 1, id);

    stdx::optional<Producto> opt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Producto p;
        p.id     = sqlite3_column_int(stmt, 0);
        p.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.precio = sqlite3_column_double(stmt, 2);
        p.stock  = sqlite3_column_int(stmt, 3);
        opt = p;
    }
    sqlite3_finalize(stmt);
    return opt;
}

// Marca activo = 0
bool DBHandler::deleteProduct(int id) {
    const char* sql =
        "UPDATE productos "
        "SET activo = 0 "
        "WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// Graba una venta: crea transacción + items_transaccion + ajusta stock
bool DBHandler::recordSale(int producto_id, int cantidad, const std::string& fecha) {
    // Recuperar precio unitario
    auto optP = getProduct(producto_id);
    if (!optP) return false;
    double pu = optP->precio;
    double total = pu * cantidad;

    // Empezar transacción
    char* err = nullptr;
    if (sqlite3_exec(db_, "BEGIN;", nullptr, nullptr, &err) != SQLITE_OK) {
        sqlite3_free(err);
        return false;
    }

    // 1) insert en transacciones
    const char* sql1 =
        "INSERT INTO transacciones (tipo, fecha, total) "
        "VALUES ('venta', ?, ?);";
    sqlite3_stmt* st1 = nullptr;
    if (sqlite3_prepare_v2(db_, sql1, -1, &st1, nullptr) != SQLITE_OK) {
        sqlite3_finalize(st1);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_bind_text (st1, 1, fecha.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(st1, 2, total);
    if (sqlite3_step(st1) != SQLITE_DONE) {
        sqlite3_finalize(st1);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(st1);

    // Obtener el último ID de transacción
    int trans_id = static_cast<int>(sqlite3_last_insert_rowid(db_));

    // 2) insert en items_transaccion
    const char* sql2 =
        "INSERT INTO items_transaccion "
        "(transaccion_id, producto_id, cantidad, precio_unitario, total_item) "
        "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* st2 = nullptr;
    if (sqlite3_prepare_v2(db_, sql2, -1, &st2, nullptr) != SQLITE_OK) {
        sqlite3_finalize(st2);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_bind_int   (st2, 1, trans_id);
    sqlite3_bind_int   (st2, 2, producto_id);
    sqlite3_bind_int   (st2, 3, cantidad);
    sqlite3_bind_double(st2, 4, pu);
    sqlite3_bind_double(st2, 5, total);
    if (sqlite3_step(st2) != SQLITE_DONE) {
        sqlite3_finalize(st2);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(st2);

    // 3) ajustar stock
    if (!updateStock(producto_id, -cantidad)) {
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    // Commit
    if (sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &err) != SQLITE_OK) {
        sqlite3_free(err);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    return true;
}

// Lista transacciones entre dos fechas (formato YYYY-MM-DD)
std::vector<Transaccion> DBHandler::listTransactions() {
    const char* sql =
      "SELECT id, tipo, fecha, total FROM transacciones;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Transaccion> res;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
      sqlite3_finalize(stmt);
      return res;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      Transaccion t;
      t.id    = sqlite3_column_int   (stmt, 0);
      t.tipo  = reinterpret_cast<const char*>(
                  sqlite3_column_text(stmt, 1));
      t.fecha = reinterpret_cast<const char*>(
                  sqlite3_column_text(stmt, 2));
      t.total = sqlite3_column_double(stmt, 3);
      res.push_back(t);
    }
    sqlite3_finalize(stmt);
    return res;
}

// Estadísticas: total ventas (items), ingresos y promedio por venta
SalesStats DBHandler::getSalesStats(const std::string& start, const std::string& end) {
    const char* sql =
        "SELECT COUNT(it.id), SUM(it.total_item), AVG(it.total_item) "
        "FROM items_transaccion it "
        "JOIN transacciones t ON it.transaccion_id = t.id "
        "WHERE UPPER(t.tipo) = 'VENTA' "
        "  AND t.fecha BETWEEN ? AND ?;";
    sqlite3_stmt* stmt = nullptr;
    SalesStats st{0,0.0,0.0};

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return st;
    }

    sqlite3_bind_text(stmt, 1, start.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, end.c_str(),   -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        st.totalCount   = sqlite3_column_int(stmt, 0);
        st.totalRevenue = sqlite3_column_double(stmt, 1);
        st.avgPerSale   = sqlite3_column_double(stmt, 2);
    }

    sqlite3_finalize(stmt);
    return st;
}

// Productos con stock bajo un umbral
std::vector<Producto> DBHandler::listLowStock(int threshold) {
    const char* sql =
        "SELECT id, nombre, precio, stock "
        "FROM productos "
        "WHERE activo = 1 AND stock < ?;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Producto> lows;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return lows;
    }
    sqlite3_bind_int(stmt, 1, threshold);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Producto p;
        p.id     = sqlite3_column_int(stmt, 0);
        p.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.precio = sqlite3_column_double(stmt, 2);
        p.stock  = sqlite3_column_int(stmt, 3);
        lows.push_back(p);
    }
    sqlite3_finalize(stmt);
    return lows;
}
