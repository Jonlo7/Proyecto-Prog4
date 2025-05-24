#include "admin_net.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <cstring>

AdminNet::AdminNet(const std::string& h, uint16_t p)
    : host_(h), port_(p), sock_(INVALID_SOCKET) {}

bool AdminNet::connectServer() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup falló\n";
        return false;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) {
        std::cerr << "socket() falló, código " << WSAGetLastError() << "\n";
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port_);
    serv_addr.sin_addr.s_addr = inet_addr(host_.c_str());
    if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Dirección inválida: " << host_ << "\n";
        closesocket(sock_);
        sock_ = INVALID_SOCKET;
        return false;
    }

    if (connect(sock_, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "connect() falló, código " << WSAGetLastError() << "\n";
        closesocket(sock_);
        sock_ = INVALID_SOCKET;
        return false;
    }

    return true;
}

std::string AdminNet::sendCommand(const std::string& cmd) {
    if (sock_ == INVALID_SOCKET) {
        std::cerr << "Error: no conectado al servidor\n";
        return "";
    }

    int sent = send(sock_, cmd.c_str(), static_cast<int>(cmd.size()), 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "send() falló, código " << WSAGetLastError() << "\n";
        return "";
    }

    char buffer[2048];
    int received = recv(sock_, buffer, sizeof(buffer) - 1, 0);
    if (received == SOCKET_ERROR) {
        std::cerr << "recv() falló, código " << WSAGetLastError() << "\n";
        return "";
    }
    if (received == 0) {
        return "";
    }

    buffer[received] = '\0';
    return std::string(buffer);
}

void AdminNet::closeConn() {
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
        sock_ = INVALID_SOCKET;
    }
    WSACleanup();
}
