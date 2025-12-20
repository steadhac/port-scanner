#pragma once
#include <string>
#include <openssl/x509.h>

/**
 * @brief Holds information about an X.509 certificate.
 */
struct CertInfo {
    bool valid;             ///< True if a certificate was retrieved.
    std::string subject;    ///< Certificate subject.
    std::string issuer;     ///< Certificate issuer.
    std::string not_after;  ///< Expiry date as a string.
    bool self_signed;       ///< True if the certificate is self-signed.
};

/**
 * @brief Attempts a TLS handshake and retrieves certificate information.
 * 
 * @param host The hostname or IP address to connect to.
 * @param port The port to connect to (usually 443 for HTTPS).
 * @param timeout_sec Connection timeout in seconds (default: 3).
 * @return CertInfo Structure containing certificate details.
 */
CertInfo get_cert_info(const std::string& host, int port, int timeout_sec = 3);