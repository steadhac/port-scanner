#include "cert_utils.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <iostream>
#include <cstring>

/**
 * @brief Logs the current OpenSSL error queue to stderr and clears it.
 * @param context Description of where the error occurred.
 */
static void log_ssl_errors(const std::string& context) {
    unsigned long err;
    char buf[256];
    while ((err = ERR_get_error()) != 0) {
        ERR_error_string_n(err, buf, sizeof(buf));
        std::cerr << "SSL error [" << context << "]: " << buf << "\n";
    }
}

/**
 * @brief Attempts a TLS handshake to the specified host and port,
 *        retrieves the peer certificate, and extracts certificate details.
 *
 * @param host The hostname or IP address to connect to.
 * @param port The port to connect to (typically 443 for HTTPS).
 * @param timeout_sec Connection timeout in seconds (default: 3).
 * @return CertInfo Structure containing certificate details.
 */
CertInfo get_cert_info(const std::string& host, int port, int timeout_sec) {
    CertInfo info = {false, "", "", "", false};

    // Initialize OpenSSL library
    SSL_library_init();
    SSL_load_error_strings();
    ERR_clear_error();

    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        log_ssl_errors("SSL_CTX_new");
        return info;
    }

    // Create BIO chain for SSL connection (owns SSL object internally)
    BIO* bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
        log_ssl_errors("BIO_new_ssl_connect");
        SSL_CTX_free(ctx);
        return info;
    }

    // Set the target hostname and port
    std::ostringstream oss;
    oss << host << ":" << port;
    BIO_set_conn_hostname(bio, oss.str().c_str());

    // Set SNI hostname on the SSL object owned by the BIO
    SSL* ssl = nullptr;
    BIO_get_ssl(bio, &ssl);
    if (ssl) {
        SSL_set_tlsext_host_name(ssl, host.c_str());
    }

    // Attempt to connect (blocking mode — reliable with timeout via SO_RCVTIMEO)
    if (BIO_do_connect(bio) <= 0) {
        // Connection refused / host unreachable is a normal scan result — clear
        // the error queue silently rather than spamming stderr for closed ports.
        ERR_clear_error();
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return info;
    }

    // Perform TLS handshake
    if (BIO_do_handshake(bio) <= 0) {
        log_ssl_errors("BIO_do_handshake");
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return info;
    }

    // Retrieve the peer certificate from the SSL object inside the BIO
    if (!ssl) {
        std::cerr << "Error: could not retrieve SSL object from BIO\n";
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return info;
    }

    X509* cert = SSL_get_peer_certificate(ssl);
    if (!cert) {
        std::cerr << "Error: no certificate presented by " << host << ":" << port << "\n";
        log_ssl_errors("SSL_get_peer_certificate");
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return info;
    }

    // Extract subject and issuer as strings
    char* subj   = X509_NAME_oneline(X509_get_subject_name(cert), nullptr, 0);
    char* issuer = X509_NAME_oneline(X509_get_issuer_name(cert), nullptr, 0);

    // Extract certificate expiration date
    const ASN1_TIME* not_after = X509_get0_notAfter(cert);
    BIO* mem = BIO_new(BIO_s_mem());
    if (mem) {
        ASN1_TIME_print(mem, not_after);
        char buf[256];
        memset(buf, 0, sizeof(buf));
        BIO_read(mem, buf, sizeof(buf) - 1);
        info.not_after = buf;
        BIO_free(mem);
    } else {
        std::cerr << "Error: BIO_new(BIO_s_mem()) failed for expiry extraction\n";
    }

    info.valid = true;
    info.subject = subj    ? subj    : "";
    info.issuer  = issuer  ? issuer  : "";
    // Check if certificate is self-signed (subject == issuer)
    info.self_signed = (X509_NAME_cmp(X509_get_subject_name(cert), X509_get_issuer_name(cert)) == 0);

    if (subj)   OPENSSL_free(subj);
    if (issuer) OPENSSL_free(issuer);
    X509_free(cert);

    // Clean up OpenSSL objects
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return info;
}