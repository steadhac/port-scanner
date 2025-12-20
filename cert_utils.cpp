#include "cert_utils.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <iostream>

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
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) return info;

    // Create SSL object and BIO for connection
    SSL* ssl = SSL_new(ctx);
    BIO* bio = BIO_new_ssl_connect(ctx);

    // Set the target hostname and port
    std::ostringstream oss;
    oss << host << ":" << port;
    BIO_set_conn_hostname(bio, oss.str().c_str());

    // Set non-blocking mode (optional, for timeout handling)
    BIO_set_nbio(bio, 1);

    // Attempt to connect
    if (BIO_do_connect(bio) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return info;
    }

    // Retrieve the peer certificate
    X509* cert = SSL_get_peer_certificate(ssl);
    if (cert) {
        // Extract subject and issuer as strings
        char* subj = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        char* issr = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);

        // Extract certificate expiration date
        const ASN1_TIME* not_after = X509_get0_notAfter(cert);
        BIO* mem = BIO_new(BIO_s_mem());
        ASN1_TIME_print(mem, not_after);
        char buf[256];
        memset(buf, 0, sizeof(buf));
        BIO_read(mem, buf, sizeof(buf) - 1);

        info.valid = true;
        info.subject = subj ? subj : "";
        info.issuer = issr ? issr : "";
        info.not_after = buf;
        // Check if certificate is self-signed (subject == issuer)
        info.self_signed = (X509_NAME_cmp(X509_get_subject_name(cert), X509_get_issuer_name(cert)) == 0);

        if (subj) OPENSSL_free(subj);
        if (issr) OPENSSL_free(issr);
        BIO_free(mem);
        X509_free(cert);
    }

    // Clean up OpenSSL objects
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return info;
}