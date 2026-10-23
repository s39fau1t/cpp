#include <iostream>

#include <nodecpp/nodecpp.h>
#include <nodecpp/fs.h>

//TODO: Fix EventEmitter (is it fine how it is?)

void cb(std::optional<nodecpp::Error> error, int fd) {
	if(error) {
		if(error->code == nodecpp::ErrorCode::ENOENT) {
			std::cout << "enoent!!!\n";
		}

		throw *error;
	}

	std::cout << fd << '\n';
}

int main() {
	fs::open("../main.cpp", cb);

	int a = 5;

	fs::open("../main.cpp", [&a](std::optional<nodecpp::Error> error, int fd) {
		if(error) {
			throw error.value();
		}

		a += 100;

		std::cout << fd << '\n';
		std::cout << a << '\n';
	});

	nodecpp::run();

	/*
	EventEmitter<
		Event<"hello">,
		Event<"wow", std::string_view>,
		Event<"test", float>,
		Event<"bad">,
		Event<"listenerCount">,
		Event<"removeAll">,
		Event<"once">
	> emitter;

	//Multiple listeners
	emitter.on<"hello">([]() {
		std::cout << "Hello, World!\n";
	});

	emitter.on<"hello">([] () {
		std::cout << "Hello2\n";
	});

	//Arguments
	emitter.on<"wow">([](std::string_view a) {
		std::cout << a << '\n';
	});

	//Capturing
	int timesCalled = 0;

	emitter.on<"wow">([&timesCalled](std::string_view a) {
		std::cout << "This event has been called " << ++timesCalled << " time(s)! Also the message is " << a << '\n';
	});

	auto func = []() {
		std::cout << "Hello, world!\n\n";
	};

	emitter.on<"listenerCount">(func);
	emitter.on<"listenerCount">(func);

	emitter.on<"listenerCount">([]() {
		std::cout << "wowowowow\n";
	});

	emitter.emit<"listenerCount">();

	std::cout << "Testing here!\n";

	std::cout << emitter.listenerCount<"listenerCount">() << '\n';

	emitter.emit<"hello">();
	emitter.emit<"wow">("Wow!");
	emitter.emit<"wow">("Wow2!");
	//No listeners
	emitter.emit<"test">(1.0f);
	//Multiple listener types
	emitter.emit<"bad">();

	std::vector<std::string_view> names = emitter.eventNames();

	for(std::string_view name : names) {
		std::cout << name << '\n';
	}

	emitter.on<"removeAll">([]() {
		std::cout << "removeAll!\n";
	});

	emitter.emit<"removeAll">();

	emitter.removeAllListeners<"removeAll">();

	emitter.emit<"removeAll">();
	emitter.emit<"hello">();

	emitter.removeAllListeners();

	emitter.emit<"removeAll">();
	emitter.emit<"hello">();
	emitter.emit<"bad">();

	emitter.once<"once">([] {
		std::cout << "Once!\n";
	});

	emitter.on<"once">([] {
		std::cout << "Always!\n";
	});

	emitter.emit<"once">();
	emitter.emit<"once">();
	*/
}
