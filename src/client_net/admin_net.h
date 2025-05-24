// client_net/admin_net.h
#ifndef ADMIN_NET_H
#define ADMIN_NET_H

#include <string>
#include <cstdint>

class AdminNet {
public:
    
    AdminNet(const std::string& host, uint16_t port);

    bool connectServer();

    std::string sendCommand(const std::string& cmd);

    void closeConn();

private:
    std::string host_;
    uint16_t    port_;
    int         sock_;
};

#endif
