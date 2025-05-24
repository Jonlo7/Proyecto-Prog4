// client_net/admin_net.h
#ifndef ADMIN_NET_H
#define ADMIN_NET_H

#include <string>
#include <cstdint>

class AdminNet {
public:
    // host: dirección IP o nombre de host del servidor
    // port: puerto TCP donde escucha el servidor
    AdminNet(const std::string& host, uint16_t port);

    // Abre la conexión al servidor remoto
    bool connectServer();

    // Envía un comando formateado y devuelve la respuesta completa
    std::string sendCommand(const std::string& cmd);

    // Cierra la conexión si está abierta
    void closeConn();

private:
    std::string host_;
    uint16_t    port_;
    int         sock_;
};

#endif // ADMIN_NET_H
