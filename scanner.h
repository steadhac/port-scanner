#include <string>
/**
 * @brief Enum representing the status of a port.
 * 
 * OPEN: The port is open and accepting connections.
 * CLOSED: The port is closed and not accepting connections.
 * FILTERED: The port is filtered (e.g., by a firewall) and no response was received.
 */
enum class PortStatus { OPEN, CLOSED, FILTERED };

/**
 * @brief Attempts to connect to a TCP port on a host.
 * @param host The hostname or IP address to scan.
 * @param port The port number to scan.
 * @param timeout_sec Timeout in seconds for the connection attempt (default: 3).
 * @return PortStatus indicating if the port is OPEN, CLOSED, or FILTERED.
 */
PortStatus scan_port(const std::string& host, int port, int timeout_sec = 3);

/**
 * @brief Converts PortStatus enum to a human-readable string.
 * @param status The PortStatus value to convert.
 * @return A string representation of the port status.
 */
std::string status_to_string(PortStatus status);