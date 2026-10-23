#pragma once

#include "enums.h"
#include "sockaddr.h"

class Socket {
	int fd;

public:
	//TODO: Allow protocol to be set
	Socket(ProtocolFamily domain, Socktype type);

	void bind(const Sockaddr &address);
	void connect(const Sockaddr &address);
	void listen(int backlog);
	Socket accept();
	void send(const void* buffer, size_t length);
	//TODO: recv
};
