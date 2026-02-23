### Port Scanner
A simple C++ port scanner that checks the status of TCP ports on specified hosts. For HTTPS ports, it retrieves and displays certificate information.

### Features
Scan custom hosts and port ranges via command-line arguments
Default scan of approved hosts and secure ports (from config.h)
Detects port status: OPEN, CLOSED, or FILTERED
Retrieves and displays TLS certificate details for HTTPS ports
Cross-platform (tested on macOS and Linux)
Clean, Doxygen-documented code
### How to Execute

All commands below must be run from inside the `port_scanner_cpp/` directory:
```bash
cd port_scanner_cpp
```

#### 1. Build first (if not already built)
```bash
mkdir -p build && cd build
cmake ..
cmake --build .
cd ..
```
The binary is produced at `port_scanner_cpp/build/port_scanner`.

#### 2. Default scan
Scans `127.0.0.1` and `localhost` on the secure ports defined in `config.h` (SSH 22, RDP 3389, VNC 5900, HTTPS 443):
```bash
./build/port_scanner
```

#### 3. Scan a custom host and port range
```bash
./build/port_scanner <host> <start_port> <end_port>
```

**Examples:**
```bash
# Scan a single port
./build/port_scanner 127.0.0.1 5432 5432

# Scan a port range
./build/port_scanner 127.0.0.1 1 1024

# Scan all ports (takes several minutes)
./build/port_scanner 127.0.0.1 1 65535

# Scan a named host
./build/port_scanner localhost 80 80
```

#### 4. Show only open ports
Results go to stdout, errors go to stderr — filter them independently:
```bash
./build/port_scanner 127.0.0.1 1 65535 2>/dev/null | grep OPEN
```

#### 5. Error cases the scanner catches
```bash
# Wrong number of arguments → usage message
./build/port_scanner 127.0.0.1 80

# Non-numeric port
./build/port_scanner 127.0.0.1 abc 443

# Port out of range
./build/port_scanner 127.0.0.1 0 99999

# Start port greater than end port
./build/port_scanner 127.0.0.1 9000 80
```

### Output Example
```bash
Host: 127.0.0.1 Port: 8000 Status: OPEN
Host: 127.0.0.1 Port: 22 (SSH) Status: CLOSED
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
