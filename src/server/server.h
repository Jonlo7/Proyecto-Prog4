#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "db_handler.h"

class Server {
public:
    Server(const std::string& db_path, uint16_t port);
    ~Server();

    bool init();
    void run();

private:
    std::string handleCommand(const std::string& cmd);

    std::string db_path_;
    uint16_t port_;
    int listen_sock_;
    DBHandler db_;
};

#endif
