#include "addrinfo.h"

#include <cstring>
#include <iostream>
#include <system_error>

AddrInfo::Flags operator|(AddrInfo::Flags lhs, AddrInfo::Flags rhs) {
	return static_cast<AddrInfo::Flags>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

AddrInfo::Flags &operator|=(AddrInfo::Flags &lhs, AddrInfo::Flags rhs) {
	lhs = lhs | rhs;

	return lhs;
}

AddrInfo::AddrInfo(addrinfo* info) : value(info) {}

AddressFamily AddrInfo::family() const {
	return static_cast<AddressFamily>(value->ai_family);
}

Socktype AddrInfo::socktype() const {
	return static_cast<Socktype>(value->ai_socktype);
}

int AddrInfo::protocol() const {
	return value->ai_protocol;
}

Sockaddr AddrInfo::addr() const {
	return {*value->ai_addr, value->ai_addrlen};
}

const char* AddrInfo::cannonname() const {
	return value->ai_canonname;
}

AddrInfoList::iterator::iterator(addrinfo* value) : value(value) {}

AddrInfo AddrInfoList::iterator::operator*() {
	return this->value;
}

bool AddrInfoList::iterator::operator!=(const iterator &that) {
	return value != that.value;
}

AddrInfoList::iterator &AddrInfoList::iterator::operator++() {
	this->value = this->value->ai_next;

	return *this;
}

AddrInfoList::AddrInfoList(const char* hostname, const char* servname, AddressFamily family, Socktype socktype, AddrInfo::Flags flags) {
	addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));

	hints.ai_family = static_cast<int>(family);
	hints.ai_socktype = static_cast<int>(socktype);
	hints.ai_flags = static_cast<int>(flags);

	int status = getaddrinfo(hostname, servname, &hints, &first);

	if(status != 0) {
		throw std::system_error(status, std::system_category(), gai_strerror(status));
	}
}

AddrInfoList::iterator AddrInfoList::begin() const {
	return first;
}

AddrInfoList::iterator AddrInfoList::end() const {
	return nullptr;
}

AddrInfoList::~AddrInfoList() {
	freeaddrinfo(first);
}
