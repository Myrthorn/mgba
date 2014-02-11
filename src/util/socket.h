#ifndef SOCKET_H
#define SOCKET_H

#ifdef _WIN32
#include <winsock2.h>

typedef SOCKET Socket;
#else
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int Socket;
#endif


static inline void SocketSubsystemInitialize() {
#ifdef _WIN32
	WSAStartup(MAKEWORD(2, 2), 0);
#endif
}

static inline ssize_t SocketSend(Socket socket, const void* buffer, size_t size) {
	return write(socket, buffer, size);
}

static inline ssize_t SocketRecv(Socket socket, void* buffer, size_t size) {
	return read(socket, buffer, size);
}

static inline Socket SocketOpenTCP(int port, uint32_t bindAddress) {
	Socket sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		return sock;
	}

	struct sockaddr_in bindInfo = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = {
			.s_addr = htonl(bindAddress)
		}
	};
	int err = bind(sock, (const struct sockaddr*) &bindInfo, sizeof(struct sockaddr_in));
	if (err) {
		close(sock);
		return -1;
	}
	return sock;
}

static inline Socket SocketListen(Socket socket, int queueLength) {
	return listen(socket, queueLength);
}

static inline Socket SocketAccept(Socket socket, struct sockaddr* restrict address, socklen_t* restrict addressLength) {
	return accept(socket, address, addressLength);
}

static inline int SocketClose(Socket socket) {
	return close(socket) >= 0;
}

static inline int SocketSetBlocking(Socket socket, int blocking) {
#ifdef _WIN32
	blocking = !blocking;
	return ioctlsocket(socket, FIONBIO, &blocking) == NO_ERROR;
#else
	int flags = fcntl(socket, F_GETFL);
	if (flags == -1) {
		return 0;
	}
	if (blocking) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}
	return fcntl(socket, F_SETFL, flags) >= 0;
#endif
}

static inline int SocketSetTCPPush(Socket socket, int push) {
	return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*) &push, sizeof(int)) >= 0;
}

#endif
