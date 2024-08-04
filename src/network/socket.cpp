/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "socket.h"

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "util/string.h"
#include "util/numeric.h"
#include "constants.h"
#include "debug.h"
#include "log.h"

// Cross-platform socket initialization
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define LAST_SOCKET_ERR() WSAGetLastError()
#define SOCKET_ERR_STR(e) std::to_string(e)
typedef int socklen_t;
#else
#define LAST_SOCKET_ERR() (errno)
#define SOCKET_ERR_STR(e) strerror(e)
#endif

bool socket_enable_debug_output = false; // yuck
static bool g_sockets_initialized = false;

// Initialize sockets
void sockets_init() {
#ifdef _WIN32
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR)
		throw SocketException("WSAStartup failed");
#endif
	g_sockets_initialized = true;
}

// Cleanup sockets
void sockets_cleanup() {
#ifdef _WIN32
	WSACleanup();
#endif
	g_sockets_initialized = false;
}

/*
	UDPSocket
*/

UDPSocket::UDPSocket(bool ipv6) {
	init(ipv6, false);
}

// Initialize socket with option for no exceptions
bool UDPSocket::init(bool ipv6, bool noExceptions) {
	if (!g_sockets_initialized) {
		verbosestream << "Sockets not initialized" << '\n';
		return false;
	}

	if (m_handle >= 0) {
		auto msg = "Cannot initialize socket twice";
		verbosestream << msg << '\n';
		if (noExceptions)
			return false;
		throw SocketException(msg);
	}

	m_addr_family = ipv6 ? AF_INET6 : AF_INET;
	m_handle = socket(m_addr_family, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_enable_debug_output) {
		tracestream << "UDPSocket(" << m_handle
					<< ")::UDPSocket(): ipv6 = " << (ipv6 ? "true" : "false")
					<< '\n';
	}

	if (m_handle < 0) {
		if (noExceptions) {
			return false;
		}
		throw SocketException("Failed to create socket: error " +
				std::string(SOCKET_ERR_STR(LAST_SOCKET_ERR())));
	}

	setTimeoutMs(0);

	if (m_addr_family == AF_INET6) {
		int value = 0;
		setsockopt(m_handle, IPPROTO_IPV6, IPV6_V6ONLY,
				reinterpret_cast<char *>(&value), sizeof(value));
	}

	return true;
}

// Socket destructor
UDPSocket::~UDPSocket() {
	if (socket_enable_debug_output) {
		tracestream << "UDPSocket(" << m_handle << ")::~UDPSocket()" << '\n';
	}

	if (m_handle >= 0) {
#ifdef _WIN32
		closesocket(m_handle);
#else
		close(m_handle);
#endif
	}
}

// Bind socket to address
void UDPSocket::Bind(Address addr) {
	if (socket_enable_debug_output) {
		tracestream << "UDPSocket(" << m_handle
					<< ")::Bind(): " << addr.serializeString() << ":"
					<< addr.getPort() << '\n';
	}

	if (addr.getFamily() != m_addr_family) {
		const char *errmsg = "Socket and bind address families do not match";
		errorstream << "Bind failed: " << errmsg << '\n';
		throw SocketException(errmsg);
	}

	int ret = 0;

	if (m_addr_family == AF_INET6) {
		struct sockaddr_in6 address = {};
		address.sin6_family = AF_INET6;
		address.sin6_addr = addr.getAddress6();
		address.sin6_port = htons(addr.getPort());

		ret = bind(m_handle, reinterpret_cast<const struct sockaddr *>(&address),
				sizeof(address));
	} else {
		struct sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_addr = addr.getAddress();
		address.sin_port = htons(addr.getPort());

		ret = bind(m_handle, reinterpret_cast<const struct sockaddr *>(&address),
				sizeof(address));
	}

	if (ret < 0) {
		tracestream << m_handle << ": Bind failed: "
					<< SOCKET_ERR_STR(LAST_SOCKET_ERR()) << '\n';
		throw SocketException("Failed to bind socket");
	}
}

// Send data to destination address
void UDPSocket::Send(const Address &destination, const void *data, int size) {
	if (INTERNET_SIMULATOR && (myrand() % INTERNET_SIMULATOR_PACKET_LOSS == 0)) {
		tracestream << "UDPSocket::Send(): INTERNET_SIMULATOR: dumping packet." << '\n';
		return;
	}

	if (destination.getFamily() != m_addr_family)
		throw SendFailedException("Address family mismatch");

	int sent;
	if (m_addr_family == AF_INET6) {
		struct sockaddr_in6 address = {};
		address.sin6_family = AF_INET6;
		address.sin6_addr = destination.getAddress6();
		address.sin6_port = htons(destination.getPort());

		sent = sendto(m_handle, reinterpret_cast<const char *>(data), size, 0,
				reinterpret_cast<struct sockaddr *>(&address), sizeof(address));
	} else {
		struct sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_addr = destination.getAddress();
		address.sin_port = htons(destination.getPort());

		sent = sendto(m_handle, reinterpret_cast<const char *>(data), size, 0,
				reinterpret_cast<struct sockaddr *>(&address), sizeof(address));
	}

	if (sent != size) {
		throw SendFailedException("Failed to send packet");
	}
}

// Receive data from sender address
int UDPSocket::Receive(Address &sender, void *data, int size) {
	assert(m_timeout_ms >= 0);
	if (!WaitData(m_timeout_ms)) {
		return -1;
	}

	size = std::max(size, 0);

	int received;
	if (m_addr_family == AF_INET6) {
		struct sockaddr_in6 address = {};
		socklen_t address_len = sizeof(address);

		received = recvfrom(m_handle, reinterpret_cast<char *>(data), size, 0,
				reinterpret_cast<struct sockaddr *>(&address), &address_len);

		if (received < 0) {
			return -1;
		}

		u16 address_port = ntohs(address.sin6_port);
		const auto *bytes = reinterpret_cast<IPv6AddressBytes *>(address.sin6_addr.s6_addr);
		sender = Address(bytes, address_port);
	} else {
		struct sockaddr_in address = {};
		socklen_t address_len = sizeof(address);

		received = recvfrom(m_handle, reinterpret_cast<char *>(data), size, 0,
				reinterpret_cast<struct sockaddr *>(&address), &address_len);

		if (received < 0) {
			return -1;
		}

		u32 address_ip = ntohl(address.sin_addr.s_addr);
		u16 address_port = ntohs(address.sin_port);

		sender = Address(address_ip, address_port);
	}

	if (socket_enable_debug_output) {
		tracestream << m_handle << " <- ";
		sender.print(tracestream);
		tracestream << ", size=" << received << ", data=";

		for (int i = 0; i < std::min(received, 20); i++) {
			if (i % 2 == 0)
				tracestream << " ";
			tracestream << std::hex << std::setw(2) << std::setfill('0')
						<< static_cast<unsigned int>(reinterpret_cast<unsigned char *>(data)[i]);
		}
		if (received > 20) {
			tracestream << "...";
		}
		tracestream << '\n';
	}

	return received;
}

// Set timeout in milliseconds
void UDPSocket::setTimeoutMs(int timeout_ms) {
	m_timeout_ms = timeout_ms;
}

// Wait for data with timeout
bool UDPSocket::WaitData(int timeout_ms) {
	timeout_ms = std::max(timeout_ms, 0);

#ifdef _WIN32
	WSAPOLLFD pfd;
	pfd.fd = m_handle;
	pfd.events = POLLRDNORM;

	int result = WSAPoll(&pfd, 1, timeout_ms);
#else
	struct pollfd pfd;
	pfd.fd = m_handle;
	pfd.events = POLLIN;

	int result = poll(&pfd, 1, timeout_ms);
#endif

	if (result == 0) {
		return false; // No data
	} else if (result > 0) {
		return pfd.revents != 0; // There might be data
	}

	int e = LAST_SOCKET_ERR();

#ifdef _WIN32
	if (e == WSAEINTR || e == WSAEBADF) {
#else
	if (e == EINTR || e == EBADF) {
#endif
		return false;
	}

	tracestream << m_handle << ": poll failed: " << SOCKET_ERR_STR(e) << '\n';

	throw SocketException("poll failed");
}
