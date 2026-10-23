#include <iostream>

#include "addrinfo.h"

int main() {
	AddrInfoList infoList(nullptr, "8000", AddressFamily::Unspec, Socktype::Stream, AddrInfo::Flags::Passive);
	AddrInfo info = *infoList.begin();
}
