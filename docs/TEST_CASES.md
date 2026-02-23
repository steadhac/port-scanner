# Test Cases

---

## Existing Test Cases

**TC01:** Scanning a set of approved hosts for a set of secure ports
Iterates through all hosts in APPROVED_HOSTS and scans each port in SECURE_PORTS.

**TC02:** Reporting port status for each host/port combination
For each host and port, attempts a TCP connection and reports status: OPEN, CLOSED, or FILTERED.

**TC03:** Retrieving and displaying certificate details for HTTPS ports
For HTTPS ports, performs a TLS handshake and retrieves certificate information:
- Certificate validity
- Subject
- Issuer
- Expiry date
- Self-signed status

### Example Output
```bash
Host: 127.0.0.1 Port: 22 (SSH) Status: CLOSED
Host: 127.0.0.1 Port: 443 (HTTPS) Cert valid: no Subject:  Issuer:  Expiry:  Self-signed: no
Host: example.com Port: 443 (HTTPS) Cert valid: yes Subject: ... Issuer: ... Expiry: ... Self-signed: no
```

---

## Error Handling Test Cases

**TC04:** Wrong number of arguments
- Input: `./port_scanner 127.0.0.1 80` (only 2 args, expects 3)
- Expected: prints usage message, exits with code 1

**TC05:** Non-numeric port argument
- Input: `./port_scanner 127.0.0.1 abc 443`
- Expected: `Error: 'abc' is not a valid integer.`, exits with code 1

**TC06:** Port out of range — zero
- Input: `./port_scanner 127.0.0.1 0 80`
- Expected: `Error: port 0 is out of range (1-65535).`, exits with code 1

**TC07:** Port out of range — above 65535
- Input: `./port_scanner 127.0.0.1 80 99999`
- Expected: `Error: port 99999 is out of range (1-65535).`, exits with code 1

**TC08:** Reversed port range
- Input: `./port_scanner 127.0.0.1 9000 80`
- Expected: `Error: start_port (9000) must be <= end_port (80).`, exits with code 1

**TC09:** Unresolvable hostname
- Input: `./port_scanner notareahost.invalid 80 80`
- Expected: `Error: could not resolve host 'notareahost.invalid' - ...` printed to stderr, port reported as CLOSED

**TC10:** Single valid port — known open
- Input: `./port_scanner 127.0.0.1 <port> <port>` where a service is running
- Expected: `Status: OPEN`

**TC11:** Single valid port — known closed
- Input: `./port_scanner 127.0.0.1 19999 19999` (nothing listening)
- Expected: `Status: CLOSED`

**TC12:** Filtered port (firewall timeout)
- Input: scan a port on a remote host behind a firewall
- Expected: `Status: FILTERED` after timeout_sec seconds

---

## Process Identification Test Cases
*(Future feature — see CONCEPTS.md)*

**TC13:** Open port shows owning process name
- Input: scan a port with a known running service (e.g., PostgreSQL on 5432)
- Expected: output includes process name (e.g., `postgres`), PID, and username

**TC14:** Open port owned by a different user
- Input: scan a system port owned by root (e.g., port 80 run by nginx as root)
- Expected: output shows the correct owner username, not the scanner's user

**TC15:** Ephemeral port with no stable process name
- Input: scan a high ephemeral port (e.g., 49179)
- Expected: output shows PID and best-available process name, or `unknown` if not resolvable

---

## Banner Grabbing Test Cases
*(Future feature — see CONCEPTS.md)*

**TC16:** SSH banner on port 22
- Input: scan an open SSH port
- Expected: banner starts with `SSH-2.0-` followed by the server version string

**TC17:** HTTP banner on port 80
- Input: scan an open HTTP port
- Expected: banner contains `HTTP/` response line or server header

**TC18:** Port with no banner (silent service)
- Input: scan a port that accepts connections but sends nothing immediately
- Expected: after read timeout, banner shown as empty or `(no banner)`

**TC19:** Binary/non-printable banner
- Input: scan a port serving a binary protocol (e.g., MySQL on 3306)
- Expected: banner displayed as hex or sanitized printable characters, no crash

---

## Service Name Resolution Test Cases
*(Future feature — see CONCEPTS.md)*

**TC20:** Known port resolves to IANA name
- Input: scan port 5432
- Expected: output includes service label `postgresql`

**TC21:** Known port resolves to IANA name — Redis
- Input: scan port 6379
- Expected: output includes service label `redis`

**TC22:** Unknown port shows numeric only
- Input: scan port 34451 (no IANA entry)
- Expected: output shows port number with no service label, or `unknown`

---

## Network Interface Auto-Detection Test Cases
*(Future feature — see CONCEPTS.md)*

**TC23:** Loopback interface detected
- Expected: `lo0` / `127.0.0.1` always present in discovered interfaces

**TC24:** Wi-Fi interface detected
- Expected: `en0` with a valid IPv4 address when Wi-Fi is connected

**TC25:** VPN tunnel interface detected
- Expected: `utun0` or similar appears when a VPN is active

**TC26:** No interfaces found — graceful failure
- Input: simulate `getifaddrs()` failure (mock/test environment)
- Expected: error message to stderr, exits cleanly with code 1

---

## IPv6 Scanning Test Cases
*(Future feature — see CONCEPTS.md)*

**TC27:** Scan IPv6 loopback
- Input: `./port_scanner ::1 80 80`
- Expected: correct OPEN/CLOSED result for port 80 on the IPv6 loopback

**TC28:** Dual-stack host resolves to both IPv4 and IPv6
- Input: scan `localhost` with IPv6 support enabled
- Expected: results shown for both `127.0.0.1` and `::1`

**TC29:** IPv6-only connection detected
- Input: scan a port that only listens on IPv6
- Expected: reported as OPEN on IPv6, CLOSED on IPv4

---

## Connection State Reporting Test Cases
*(Future feature — see CONCEPTS.md)*

**TC30:** LISTEN state detected for a local service
- Input: query connection table while PostgreSQL is running
- Expected: port 5432 shown with state `LISTEN`

**TC31:** ESTABLISHED outbound connection visible
- Input: query connection table while browser is active
- Expected: one or more entries with state `ESTABLISHED` to external IPs

**TC32:** TIME_WAIT entries visible after connection close
- Input: query connection table immediately after closing a connection
- Expected: entry shows state `TIME_WAIT` for the recently closed port

---

## Latency Measurement Test Cases
*(Future feature — see CONCEPTS.md)*

**TC33:** Localhost open port latency under 1ms
- Input: scan an open port on 127.0.0.1
- Expected: reported latency is < 1ms

**TC34:** Closed port latency near zero
- Input: scan a closed port on 127.0.0.1
- Expected: latency near zero (connection refused immediately)

**TC35:** Filtered port latency equals timeout
- Input: scan a firewalled port with 3s timeout
- Expected: reported latency is approximately 3000ms

---

## TLS Version and Cipher Detection Test Cases
*(Future feature — see CONCEPTS.md)*

**TC36:** TLS 1.3 negotiated for modern server
- Input: scan a local service configured for TLS 1.3 only
- Expected: output shows `TLSv1.3`

**TC37:** TLS 1.2 negotiated for older server
- Input: scan a local service configured for TLS 1.2
- Expected: output shows `TLSv1.2`

**TC38:** Cipher suite reported correctly
- Input: scan any HTTPS port
- Expected: output includes the negotiated cipher name (e.g., `TLS_AES_256_GCM_SHA384`)

---

## UDP Scanning Test Cases
*(Future feature — see CONCEPTS.md)*

**TC39:** mDNS port 5353 detected as open (UDP)
- Input: UDP scan port 5353 on 127.0.0.1
- Expected: reported as OPEN (macOS mDNS daemon always listens here)

**TC40:** Closed UDP port returns ICMP unreachable
- Input: UDP scan a port with no listener
- Expected: ICMP port-unreachable received, port reported as CLOSED

**TC41:** UDP scan with no response (filtered)
- Input: UDP scan a firewalled port
- Expected: no ICMP response after timeout, port reported as FILTERED

---

## JSON Output Test Cases
*(Future feature — see CONCEPTS.md)*

**TC42:** JSON output is valid JSON
- Input: `./port_scanner --json 127.0.0.1 22 22`
- Expected: output passes `jq .` without error

**TC43:** JSON contains required fields
- Input: `./port_scanner --json 127.0.0.1 22 22`
- Expected: each result object contains `host`, `port`, `status` keys

**TC44:** JSON output with HTTPS includes cert fields
- Input: `./port_scanner --json` on an HTTPS port
- Expected: result object contains `cert_valid`, `subject`, `issuer`, `expiry`, `self_signed`

**TC45:** Plain text output unchanged without flag
- Input: `./port_scanner 127.0.0.1 22 22` (no `--json`)
- Expected: output is plain text as before, not JSON

---

## Snapshot Diffing Test Cases
*(Future feature — see CONCEPTS.md)*

**TC46:** First run creates snapshot file
- Input: run scanner with `--save` flag
- Expected: snapshot file created with timestamp and results

**TC47:** Second run with no changes shows no diff
- Input: run scanner twice with `--diff` flag, no services changed between runs
- Expected: output reports `No changes detected`

**TC48:** New open port detected in diff
- Input: start a service, run scanner with `--diff`
- Expected: diff output highlights the new port as `NEW: OPEN`

**TC49:** Newly closed port detected in diff
- Input: stop a service, run scanner with `--diff`
- Expected: diff output highlights the port as `CLOSED (was OPEN)`

---

## Categorized Output Test Cases
*(Future feature — see CONCEPTS.md)*

**TC50:** Web ports grouped under Web category
- Input: scan includes ports 80 and 443
- Expected: both appear under a `Web` section in output

**TC51:** Database ports grouped correctly
- Input: scan includes ports 5432 (PostgreSQL) and 6379 (Redis)
- Expected: both appear under a `Database` section

**TC52:** Unknown ports grouped under Unknown
- Input: scan includes port 34451 (no known service)
- Expected: appears under `Unknown` or `Ephemeral` section

**TC53:** Empty category not shown
- Input: scan a range where no Remote Access ports are open
- Expected: `Remote Access` section is omitted from output entirely
