#include <cassert>
#include <iostream>
#include <thread>
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

const char* messages[] = {
	"Hello, World!",
	"How are you?",
	"Bye!"
};

int main() {
	addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));

	//IPV4 or IPV6
	hints.ai_family = AF_UNSPEC;
	//TCP protocol
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* info;

	//Get the info (just use the first one in the list)
	check(getaddrinfo("localhost", "8000", &hints, &info));

	int client = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	assert(client != -1);

	check(connect(client, info->ai_addr, info->ai_addrlen));

	for(const char* message : messages) {
		std::cout << "Sending " << message << '\n';

		send(client, message, strlen(message), 0);

		char data[packetSize];

		ssize_t size = recv(client, data, sizeof(data), 0);

		assert(size != -1);

		std::cout << "Recieved " << std::string_view(data, size) << '\n';
	}
}
