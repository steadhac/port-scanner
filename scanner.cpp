#include "scanner.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstring>
#include <errno.h>

/**
 * @brief Attempts to connect to a TCP port on a host.
 * @param host Hostname or IP address.
 * @param port TCP port number.
 * @param timeout_sec Timeout in seconds.
 * @return PortStatus (OPEN, CLOSED, FILTERED)
 */
PortStatus scan_port(const std::string& host, int port, int timeout_sec) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return PortStatus::CLOSED;

    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        close(sock);
        return PortStatus::CLOSED;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
    addr.sin_port = htons(port);

    // Set non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int res = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (res < 0 && errno != EINPROGRESS) {
        close(sock);
        return PortStatus::CLOSED;
    }

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    res = select(sock + 1, nullptr, &fdset, nullptr, &tv);
    if (res == 1) {
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        close(sock);
        if (so_error == 0) return PortStatus::OPEN;
        else return PortStatus::CLOSED;
    } else if (res == 0) {
        close(sock);
        return PortStatus::FILTERED; // Timeout
    } else {
        close(sock);
        return PortStatus::CLOSED;
    }
}

/**
 * @brief Converts PortStatus enum to a string.
 * @param status PortStatus value.
 * @return String representation.
 */
std::string status_to_string(PortStatus status) {
    switch (status) {
        case PortStatus::OPEN: return "OPEN";
        case PortStatus::CLOSED: return "CLOSED";
        case PortStatus::FILTERED: return "FILTERED";
        default: return "UNKNOWN";
    }
}