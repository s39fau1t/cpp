#pragma once

#include <sys/socket.h>

/*
Only provide enum members for things related to networking
*/

//TODO: Add more protocol and address families
enum class AddressFamily {
	Inet = AF_INET,
	Inet6 = AF_INET6,
	Unspec = AF_UNSPEC
};

enum class ProtocolFamily {
	Inet = PF_INET,
	Inet6 = PF_INET6,
};

enum class Socktype {
	Stream = SOCK_STREAM,
	DGram = SOCK_DGRAM,
	Raw = SOCK_RAW,
	Any = 0
};

//TODO: Make Protocol type maybe
