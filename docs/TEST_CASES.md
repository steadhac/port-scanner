# Test Cases Covered in main.cpp
The main.cpp file demonstrates the following test scenarios:

**TC01:** Scanning a set of approved hosts for a set of secure ports
Iterates through all hosts in APPROVED_HOSTS and scans each port in SECURE_PORTS.

**TC02:** Reporting port status for each host/port combination
For each host and port, attempts a TCP connection and reports status: OPEN, CLOSED, or FILTERED.

**TC03:** Retrieving and displaying certificate details for HTTPS ports
For HTTPS ports, performs a TLS handshake and retrieves certificate information:
Certificate validity
Subject
Issuer
Expiry date
Self-signed status

### How It Works
If run without arguments, main.cpp uses the default hosts and ports from config.h.
If run with arguments (./port_scanner <host> <start_port> <end_port>), it scans the specified host and port range.
For each scan, the result is printed to the console in a human-readable format.

### Example Output
```bash 
Host: 127.0.0.1 Port: 22 (SSH) Status: CLOSED
Host: 127.0.0.1 Port: 443 (HTTPS) Cert valid: no Subject:  Issuer:  Expiry:  Self-signed: no
Host: example.com Port: 443 (HTTPS) Cert valid: yes Subject: ... Issuer: ... Expiry: ... Self-signed: no
```
Limitations
The current implementation does not include automated unit tests or assertions.
Only the ports and hosts specified in config.h (or via command-line) are scanned.
Advanced scenarios (e.g., IPv6, large port ranges, error handling) are not covered unless added to config.h or invoked via arguments.


