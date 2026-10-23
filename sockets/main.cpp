#include <cassert>
#include <iostream>
#include <thread>
#include <string_view>
#include <cstring>
#include <cassert>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

void check(int code) {
	assert(code == 0);
}

constexpr int packetSize = 4096;

void echoClient(int client) {
	char data[packetSize];

	ssize_t size;

	while(((size = recv(client, data, sizeof(data), 0)) != 0)) {
		assert(size != -1);

		std::cout << "Recieved " << std::string_view(data, size) << '\n';

		send(client, data, size, 0);
	}

	close(client);
}

int main() {
	addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));

	//IPV4 or IPV6
	hints.ai_family = AF_UNSPEC;
	//TCP protocol
	hints.ai_socktype = SOCK_STREAM;
	//Fill in IP automatically
	hints.ai_flags = AI_PASSIVE;

	addrinfo* info;

	//Get the info (just use the first one in the list)
	check(getaddrinfo(nullptr, "8000", &hints, &info));

	int server = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	assert(server != -1);

	check(bind(server, info->ai_addr, info->ai_addrlen));

	check(listen(server, 10));

	while(true) {
		sockaddr_storage clientAddr;
		socklen_t clientAddrSize = sizeof(clientAddr);

		int client = accept(server, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);

		assert(client != -1);

		std::thread clientThread(echoClient, client);
		clientThread.detach();
	}
}
