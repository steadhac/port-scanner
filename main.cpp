#include "scanner.h"
#include "config.h"
#include "cert_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for std::atoi

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

int main(int argc, char* argv[]) {
    std::vector<std::string> hosts;
    int port_start = -1, port_end = -1;

    if (argc == 4) {
        // Usage: ./port_scanner <host> <start_port> <end_port>
        hosts.push_back(argv[1]);
        port_start = std::atoi(argv[2]);
        port_end = std::atoi(argv[3]);
    } else {
        // Use defaults from config.h
        hosts = APPROVED_HOSTS;
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