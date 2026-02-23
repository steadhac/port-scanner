# Test Plan

---

## Objective
Verify the correctness, reliability, and robustness of the port scanner across all current features and planned extensions.

---

## Scope

### Current Features
- Command-line argument parsing and validation
- TCP port scanning (OPEN, CLOSED, FILTERED)
- HTTPS certificate retrieval (validity, subject, issuer, expiry, self-signed)
- Error reporting (socket errors, DNS failures, SSL failures)

### Planned Extensions (see CONCEPTS.md)
- Process identification per open port
- Banner grabbing
- Service name resolution
- Network interface auto-detection
- IPv6 scanning
- Connection state reporting
- Latency measurement
- TLS version and cipher detection
- UDP port scanning
- JSON output mode
- Snapshot diffing
- Categorized output

---

## Test Types

| Type | Purpose |
|------|---------|
| Functional | Verify correct output for normal inputs |
| Edge case | Verify behaviour at boundaries (port 1, port 65535, empty ranges) |
| Error handling | Verify graceful failure for bad input, DNS failures, SSL errors |
| Integration | Verify end-to-end scan against live local services |
| Regression | Verify previously fixed bugs do not reappear |

---

## Test Environment Setup

### Simulating an Open Port
```bash
# Start a Python HTTP server on port 8080
python3 -m http.server 8080
```

### Simulating a Filtered Port
Use a firewall rule or scan a remote host behind a firewall to produce a timeout.

### Simulating a TLS Server (self-signed)
```bash
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 1 -nodes -subj "/CN=localhost"
openssl s_server -key key.pem -cert cert.pem -accept 4443 -www
```

### Docker (optional)
```bash
docker run -d -p 5432:5432 postgres     # PostgreSQL
docker run -d -p 6379:6379 redis        # Redis
docker run -d -p 80:80 nginx            # HTTP
```

---

## Test Cases Summary

| ID | Area | Description | TC Reference |
|----|------|-------------|-------------|
| TC01 | Core | Default scan of approved hosts and secure ports | TC01 |
| TC02 | Core | Port status reported correctly (OPEN/CLOSED/FILTERED) | TC02 |
| TC03 | Core | HTTPS certificate fields retrieved | TC03 |
| TC04 | Args | Wrong number of arguments → usage message | TC04 |
| TC05 | Args | Non-numeric port → error message | TC05 |
| TC06 | Args | Port 0 → out of range error | TC06 |
| TC07 | Args | Port > 65535 → out of range error | TC07 |
| TC08 | Args | Reversed range → error message | TC08 |
| TC09 | Error | Unresolvable hostname → DNS error to stderr | TC09 |
| TC10 | Core | Single known-open port → OPEN | TC10 |
| TC11 | Core | Single known-closed port → CLOSED | TC11 |
| TC12 | Core | Filtered port → FILTERED after timeout | TC12 |
| TC13–TC15 | Process ID | Process name, user, PID shown per open port | TC13–TC15 |
| TC16–TC19 | Banner | Service banners read and displayed | TC16–TC19 |
| TC20–TC22 | Service Names | IANA service names resolved from port numbers | TC20–TC22 |
| TC23–TC26 | Interfaces | Local network interfaces auto-detected | TC23–TC26 |
| TC27–TC29 | IPv6 | IPv6 hosts scanned correctly | TC27–TC29 |
| TC30–TC32 | Conn States | TCP connection states reported | TC30–TC32 |
| TC33–TC35 | Latency | Connection latency measured and reported | TC33–TC35 |
| TC36–TC38 | TLS | TLS version and cipher suite reported | TC36–TC38 |
| TC39–TC41 | UDP | UDP ports scanned via ICMP | TC39–TC41 |
| TC42–TC45 | JSON | JSON output mode produces valid JSON | TC42–TC45 |
| TC46–TC49 | Snapshot | Snapshot saved and diff detected | TC46–TC49 |
| TC50–TC53 | Categories | Open ports grouped by service category | TC50–TC53 |

Full test case details are in [TEST_CASES.md](TEST_CASES.md).

---

## Pass/Fail Criteria

- **Pass:** Output matches expected result exactly, exit code is correct, no crash.
- **Fail:** Wrong output, unexpected crash, exit code mismatch, or error not reported to stderr.

---

## Tools

| Tool | Purpose |
|------|---------|
| Python `http.server` | Simulate an open TCP port |
| `openssl s_server` | Simulate a local TLS server |
| Docker | Spin up real services (PostgreSQL, Redis, nginx) |
| `jq` | Validate JSON output mode |
| Shell scripts | Automate start/stop of services for diff tests |

---

## Limitations

- Automated assertions are not yet implemented — tests are run manually and output is verified by inspection.
- Process identification and UDP scanning require elevated privileges on some systems.
- Filtered port tests depend on an external firewall or a remote host and cannot be run purely locally.
- IPv6 tests require an active IPv6 interface (standard on macOS via loopback `::1`).
