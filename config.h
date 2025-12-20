#pragma once
#include <vector>
#include <string>

/**
 * @brief Configuration for a port and its associated protocol.
 */
struct PortConfig {
    int port;
    std::string protocol;
};


/**
 * @brief List of secure ports and their protocols to scan.
 */
inline const std::vector<PortConfig> SECURE_PORTS = {
    {22, "SSH"},
    {3389, "RDP"},
    {5900, "VNC"},
    {443, "HTTPS"}
};

/**
 * @brief List of approved hosts to scan.
 */
inline const std::vector<std::string> APPROVED_HOSTS = {
    "127.0.0.1",
    "localhost"
};