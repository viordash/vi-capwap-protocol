#pragma once
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

static inline std::string IpToString(uint32_t ip_addr) {
    char ip_str[INET_ADDRSTRLEN];
    struct in_addr addr = {};
    addr.s_addr = ip_addr;
    inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN);
    return ip_str;
}

static inline std::string IpToString(const in_addr &addr) {
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN);
    return ip_str;
}

static inline std::string IpToString(const sockaddr_in &sockaddr) {
    return IpToString(sockaddr.sin_addr);
}
