#include "scanner.h"
#include "config.h"
#include "cert_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for std::strtol
#include <cerrno>
#include <climits>

/**
 * @file main.cpp
 * @brief Entry point for the port scanner application.
 *
 * Usage:
 *   ./port_scanner <host> <start_port> <end_port>
 *     - Scans the specified host and port range.
 *     - Example: ./port_scanner 127.0.0.1 8000 8000
 *
 *   ./port_scanner
 *     - Scans the default APPROVED_HOSTS and SECURE_PORTS from config.h.
 *
 * For HTTPS ports, attempts a TLS handshake and retrieves certificate information.
 * For other ports, performs a basic TCP port scan.
 */

/**
 * @brief Parses a port number from a string with validation.
 * @param str String to parse.
 * @param out Output port number on success.
 * @return true on success, false if invalid.
 */
static bool parse_port(const char* str, int& out) {
    char* end;
    errno = 0;
    long val = std::strtol(str, &end, 10);
    if (errno != 0 || end == str || *end != '\0') {
        std::cerr << "Error: '" << str << "' is not a valid integer.\n";
        return false;
    }
    if (val < 1 || val > 65535) {
        std::cerr << "Error: port " << val << " is out of range (1-65535).\n";
        return false;
    }
    out = static_cast<int>(val);
    return true;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> hosts;
    int port_start = -1, port_end = -1;

    if (argc == 1) {
        // Use defaults from config.h
        hosts = APPROVED_HOSTS;
    } else if (argc == 4) {
        // Usage: ./port_scanner <host> <start_port> <end_port>
        if (!parse_port(argv[2], port_start) || !parse_port(argv[3], port_end)) {
            return 1;
        }
        if (port_start > port_end) {
            std::cerr << "Error: start_port (" << port_start
                      << ") must be <= end_port (" << port_end << ").\n";
            return 1;
        }
        hosts.push_back(argv[1]);
    } else {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << "                          # scan default hosts/ports\n"
                  << "  " << argv[0] << " <host> <start> <end>     # scan custom host and port range\n";
        return 1;
    }

    if (port_start > 0 && port_end > 0) {
        for (const auto& host : hosts) {
            for (int port = port_start; port <= port_end; ++port) {
                PortStatus status = scan_port(host, port);
                std::cout << "Host: " << host
                          << " Port: " << port
                          << " Status: " << status_to_string(status)
                          << std::endl;
            }
        }
    } else {
        for (const auto& host : hosts) {
            for (const auto& portcfg : SECURE_PORTS) {
                if (portcfg.protocol == "HTTPS") {
                    CertInfo cert = get_cert_info(host, portcfg.port);
                    std::cout << "Host: " << host
                              << " Port: " << portcfg.port
                              << " (" << portcfg.protocol << ")"
                              << " Cert valid: " << (cert.valid ? "yes" : "no")
                              << " Subject: " << cert.subject
                              << " Issuer: " << cert.issuer
                              << " Expiry: " << cert.not_after
                              << " Self-signed: " << (cert.self_signed ? "yes" : "no")
                              << std::endl;
                } else {
                    PortStatus status = scan_port(host, portcfg.port);
                    std::cout << "Host: " << host
                              << " Port: " << portcfg.port
                              << " (" << portcfg.protocol << ")"
                              << " Status: " << status_to_string(status)
                              << std::endl;
                }
            }
        }
    }
    return 0;
}