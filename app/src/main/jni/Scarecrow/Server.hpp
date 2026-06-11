#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <functional>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include "Scarecrow/MemoryTools.h"
#include "Scarecrow/OffsetsPatch.h"
#include "Scarecrow/Obfuscate.h"
#include <atomic>

inline const char* SOCKET_PATH = "/data/data/video.like/files/ipc_socket";
inline std::string processGame = "";

using CommandFunc = std::function<int()>; 
inline std::map<std::string, CommandFunc>& getCommandMap() {
    static std::map<std::string, CommandFunc> commandMap;
    return commandMap;
}

class PmtReg {
public:
    PmtReg(const std::string& name, CommandFunc func) {
        getCommandMap()[name] = func;
    }
};

#define function(Name) \
    int Name(); \
    PmtReg registrar_##Name(#Name, Name); \
    int Name() 

inline std::string readFileContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    size_t first = content.find_first_not_of(" \n\r\t");
    if (std::string::npos == first) return "";
    size_t last = content.find_last_not_of(" \n\r\t");
    return content.substr(first, (last - first + 1));
}


inline std::atomic<bool> serverRunning{true};

inline void socketServerThread() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[256];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(5588);

    if (::bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return;
    }


    struct timeval tv;
    tv.tv_sec = 1; 
    tv.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    while (serverRunning) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        std::memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received > 0) {
            std::string command(buffer, bytes_received);

            if (command.rfind("SET_PKG:", 0) == 0) {
                processGame = command.substr(8);
            } else if (command == "STOP_SERVER") {
                serverRunning = false;
            } else {
                auto& map = getCommandMap();
                if (map.count(command)) {
                    map[command]();
                }
            }
        }
        close(client_fd);
    }
    close(server_fd);
}


inline struct AutoInit {
    AutoInit() {
        std::thread listener(socketServerThread);
        listener.detach();
    }
} auto_initializer;
