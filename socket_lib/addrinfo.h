#pragma once

#include <netdb.h>

#include "enums.h"
#include "sockaddr.h"

/*
FIXME: This is trash

goals:
Don't let AddrInfo be used as hints, pass those in the AddrInfoList constructor
so don't use hint-releated properties
also don't use linked list-related properties; those are abstracted in AddrInfoList and AddrInfoList::iterator

addrinfo is this

struct addrinfo {
	int ai_flags; don't provide access
	int ai_family; change type to AddressFamily
	int ai_socktype; change type to Socktype
	int ai_protocol; if I make Protocol type, change to that, otherwise leave as int
	socklen_t ai_addrlen; Combine into Sockaddr type,
	struct sockaddr *ai_addr; Combine into Sockaddr type
	char *ai_canonname; leave as char*
	struct addrinfo *ai_next; don't provide access
};

so (change to struct)

struct AddrInfo {
	AddressFamily family;
	Socktype socktype;
	int protocol;
	Sockaddr addr;
	char* cannonname;
};

now AddrInfoList

holds addrinfo*, since AddrInfo is not a linked list
so... leave same I think

now AddrInfoList::iterator

holds addrinfo* or AddrInfo?

maybe holds AddrInfo and addrinfo*

like this

oh but how can we check if value is nullptr
and how do we make a nullptr iterator

iterator MUST hold some kind of pointer as value
so where does the value of AddrInfo* live?
in AddrInfoList?
*/

class AddrInfo {
	addrinfo* value;

public:
	//TODO: Should this be in AddrInfo or AddrInfoList, since I am hiding the hints functionality of AddrInfo
	enum class Flags {
		Addrconfig = AI_ADDRCONFIG,
		All = AI_ALL,
		CannonName = AI_CANONNAME,
		NumericHost = AI_NUMERICHOST,
		NumericServ = AI_NUMERICSERV,
		Passive = AI_PASSIVE,
		V4Mapped = AI_V4MAPPED,
		V4MappedCFG = AI_V4MAPPED_CFG,
		Default = AI_DEFAULT,
		Unusable = AI_UNUSABLE
	};

	AddrInfo(addrinfo* info);

	//TODO: Should I return references?
	AddressFamily family() const;
	Socktype socktype() const;
	int protocol() const;
	Sockaddr addr() const;
	const char* cannonname() const;
};

AddrInfo::Flags operator|(AddrInfo::Flags lhs, AddrInfo::Flags rhs);
AddrInfo::Flags &operator|=(AddrInfo::Flags &lhs, AddrInfo::Flags rhs);

class AddrInfoList {
	addrinfo* first;

public:
	class iterator {
		addrinfo* value;

	public:
		iterator(addrinfo* value);

		AddrInfo operator*();
		bool operator!=(const iterator &that);
		iterator &operator++();
	};

	//TODO: Allow ai_protocol to be set
	AddrInfoList(const char* hostname, const char* servname, AddressFamily family, Socktype socktype, AddrInfo::Flags flags);

	iterator begin() const;
	iterator end() const;

	~AddrInfoList();
};
