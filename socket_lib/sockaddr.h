#pragma once

#include <sys/socket.h>

//TODO: Somehow make this owning, but I want to avoid copying sockaddr on construction in AddrInfo::addr()
struct Sockaddr {
	const sockaddr &value;
	const socklen_t size;
};
