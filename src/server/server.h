// server.h
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "db_handler.h"

class Server {
public:
    Server(const std::string& db_path, uint16_t port);
    ~Server();

    // Inicializa socket, abre base de datos y comienza a escuchar
    bool init();
    // Bucle principal: acepta conexiones y atiende peticiones
    void run();

private:
    // Procesa un único comando recibido y devuelve la respuesta
    std::string handleCommand(const std::string& cmd);

    std::string db_path_;
    uint16_t port_;
    int listen_sock_;
    DBHandler db_;
};

#endif // SERVER_H
