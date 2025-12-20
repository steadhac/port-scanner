### Port Scanner
A simple C++ port scanner that checks the status of TCP ports on specified hosts. For HTTPS ports, it retrieves and displays certificate information.

### Features
Scan custom hosts and port ranges via command-line arguments
Default scan of approved hosts and secure ports (from config.h)
Detects port status: OPEN, CLOSED, or FILTERED
Retrieves and displays TLS certificate details for HTTPS ports
Cross-platform (tested on macOS and Linux)
Clean, Doxygen-documented code
### Usage
Scan a Custom Host and Port Range

```bash
./port_scanner <host> <start_port> <end_port>
```
Example:
```bash
./port_scanner 127.0.0.1 8000 8000
```

Scan Default Hosts and Ports
```bash
./port_scanner
```
(Default hosts and ports are defined in config.h)

### Output Example
```bash
Host: 127.0.0.1 Port: 8000 Status: OPEN
Host: example.com Port: 443 (HTTPS) Cert valid: yes Subject: ... Issuer: ... Expiry: ... Self-signed: no
```

### Build Instructions
Ensure you have a C++ compiler and OpenSSL development libraries installed.
On macOS: brew install openssl
On Ubuntu: sudo apt-get install libssl-dev
Clone this repository.
Build the project
```bash
g++ -o port_scanner main.cpp scanner.cpp cert_utils.cpp -lssl -lcrypto
```
Or use your provided Makefile if available
```bash
make
```
### File Structure
main.cpp — Entry point, argument parsing, scan orchestration
scanner.h/cpp — TCP port scanning logic
cert_utils.h/cpp — TLS certificate retrieval and parsing
config.h — Default hosts and ports

### Documentation
All public functions and data structures are documented in Doxygen style.
To generate HTML docs (if Doxygen is installed):
```bash
doxygen Doxyfile
```
### Testing
See TESTPLAN.md and TESTCASES.md for details on how to test the scanner.

### License
MIT License
