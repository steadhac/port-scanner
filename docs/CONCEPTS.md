### Project Overview
This project is a C++ port scanner that checks the status of TCP ports on specified hosts and retrieves TLS certificate information for HTTPS ports.

### Key Concepts

### 1. Port Scanning
What: Sending network requests to a range of ports on a host to determine which are open, closed, or filtered.
Why: Useful for network diagnostics, security auditing, and service discovery.

### 2. TCP Ports
**What:** Numerical identifiers for network services on a host (e.g., 22 for SSH, 80 for HTTP, 443 for HTTPS).
**Status:**
**OPEN*:** The port is accepting connections.
**CLOSED:** The port is not accepting connections.
**FILTERED:** No response (possibly blocked by a firewall).

### 3. TLS/SSL Certificate Retrieval
**What:** For HTTPS ports, the scanner attempts a TLS handshake and retrieves the server’s certificate.
**Why:** To check certificate validity, issuer, subject, expiry, and whether it’s self-signed.

---
### Code Components

### main.cpp
**Role:** Entry point. Parses command-line arguments, determines which hosts/ports to scan, and prints results.
**Logic:** If arguments are provided, scans those; otherwise, uses defaults from config.h.

### scanner.h / scanner.cpp
**Role:** Implements TCP port scanning.
**Key Functions:**
**scan_port(host, port, timeout):** Attempts to connect to a port and returns its status.
**status_to_string(status):** Converts port status to a human-readable string.

### cert_utils.h / cert_utils.cpp
**Role:** Handles TLS certificate retrieval and parsing.
**Key Functions:**
**get_cert_info(host, port, timeout):** Connects to an HTTPS port, performs a TLS handshake, and extracts certificate details.
**Data Structures:**
**CertInfo:** Holds certificate validity, subject, issuer, expiry, and self-signed status.

### config.h
**Role:** Defines default hosts and ports to scan if no command-line arguments are given.
**Typical Content:** Lists like APPROVED_HOSTS and SECURE_PORTS.

---

### How It Works (Step-by-Step)

**Startup:** User runs the program with or without arguments.
**Argument Parsing:** main.cpp decides which hosts/ports to scan.
**Scanning:**
For each host/port:
If HTTPS, calls **get_cert_info** to retrieve certificate.
Otherwise, calls **scan_port** to check port status.
**Output:** Results are printed to the console.

---

### Dependencies
**C++ Standard Library:** For core logic and I/O.
**OpenSSL:** For TLS/SSL operations and certificate parsing.

---
### Extending the Project
Add more protocols or port types.
Output results in JSON or other formats.
Integrate with CI/CD pipelines for automated network checks.

