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

### Process Identification per Open Port
**What:** For each open port, identify which process owns it — the PID, process name, and user.
**Why:** Knowing port 5432 is open is less useful than knowing it is PostgreSQL started by your own user. Uses `getpwuid`, `getpid`, and reads process tables via `sysctl`/`kinfo_proc` on macOS.

### Banner Grabbing
**What:** After connecting to an open port, read the first bytes the service sends back.
**Why:** Many services immediately announce themselves (e.g., `SSH-2.0-OpenSSH_9.x`, `220 ProFTPD`, MySQL handshake). Identifies what is actually running vs. what the port number implies.

### Service Name Resolution
**What:** Map port numbers to IANA service names using `getservbyport()` from the C standard library.
**Why:** Turns raw numbers into readable labels — port 6379 becomes `redis`, port 5432 becomes `postgresql`. Especially useful for high ephemeral ports.

### Auto-Detect Local Network Interfaces
**What:** Use `getifaddrs()` to enumerate all network interfaces (en0, lo0, utun0, etc.) and their assigned IPs.
**Why:** Instead of hardcoding `127.0.0.1`, the scanner finds every local IP automatically — Wi-Fi, VPN tunnels, Docker bridges, and loopback.

### IPv6 Scanning
**What:** Replace `gethostbyname()` with `getaddrinfo()` and add `AF_INET6` socket support alongside `AF_INET`.
**Why:** Many macOS system connections are IPv6-only (e.g., Apple Safe Browsing). The current scanner misses these entirely.

### Connection State Reporting
**What:** Enumerate the full TCP connection table using `sysctl` with `IPPROTO_TCP`/`CTL_NET` on macOS.
**Why:** TCP connections have states beyond open/closed: `LISTEN`, `ESTABLISHED`, `TIME_WAIT`, `CLOSE_WAIT`. Shows all active outbound connections (to Apple servers, Spotify, etc.), not just local listeners.

### Latency Measurement
**What:** Time each `connect()` attempt in microseconds using `gettimeofday` or `clock_gettime`.
**Why:** Open ports on localhost respond in under 1ms. Higher latency indicates system load. Over a network, this becomes a basic RTT profiler.

### TLS Version and Cipher Detection
**What:** Extend `get_cert_info` to also report the negotiated TLS version and cipher suite via `SSL_get_version()` and `SSL_get_cipher()`.
**Why:** Reveals whether local services use outdated crypto (TLS 1.0/1.1) or modern TLS 1.3.

### UDP Port Scanning
**What:** Send a UDP packet to a port and check for an ICMP port-unreachable response.
**Why:** The current scanner is TCP-only. DNS (53), mDNS (5353), DHCP, and game servers use UDP and are invisible to the current tool.

### JSON Output Mode
**What:** Add a `--json` command-line flag that emits results as structured JSON instead of plain text.
**Why:** JSON output can be piped to `jq`, saved to disk, or diffed between runs — enabling scripting and automation on top of the scanner.

### Snapshot Diffing
**What:** Save scan results with a timestamp and, on the next run, compare against the previous snapshot to highlight new or newly closed ports.
**Why:** Useful for noticing when an application opens a new port after an update, or when a service unexpectedly stops.

### Categorized Output
**What:** Group results by service category: Web, Database, Remote Access, System/IPC, Unknown.
**Why:** A flat list of 25 ports is hard to read. Grouping by category makes it immediately clear what class of service each port belongs to.

---

### Add more protocols or port types.
Output results in JSON or other formats.
Integrate with CI/CD pipelines for automated network checks.

