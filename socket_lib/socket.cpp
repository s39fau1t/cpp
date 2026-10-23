#include "socket.h"

#include <cstring>
#include <system_error>
#include <cerrno>

Socket::Socket(ProtocolFamily domain, Socktype type) {
	fd = socket(static_cast<int>(domain), static_cast<int>(type), 0);

	if(fd == -1) {
		throw std::system_error(errno, std::system_category(), std::strerror(errno));
	}
}

void Socket::bind(const Sockaddr &address) {

}

void Socket::connect(const Sockaddr &address) {

}

void Socket::listen(int backlog) {

}

Socket Socket::accept() {

}

void Socket::send(const void* buffer, size_t length) {

}
