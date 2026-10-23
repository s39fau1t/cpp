module;

#include <string>
#include <filesystem>
#include <fstream>

#include <CLI11.hpp>
#include <rang.hpp>

//TODO: Better name?
export module commands:newCmd;

import :error;

namespace newCmd {
	//TODO: More options
	struct Options {
		std::string path;
	};

	void createConfig() {
		auto config = std::ofstream{"package.toml"};
		config << R"([package]
name = )" << std::filesystem::current_path().filename() << R"(
version = "0.1.0"

[dependencies])";
	}

	void createMain() {
		auto file = std::ofstream{"src/main.cpp"};
		file << R"(#include <iostream>

int main() {
	std::cout << "Hello, World!\n";
})";
	}

	void createSrc() {
		std::filesystem::create_directory("src");

		createMain();
	}

	export void add(CLI::App& app) {
		//TODO: static Options options?
		static auto options = Options{};

		auto& cmd = *app.add_subcommand("new", "Create a new package");

		cmd.add_option("path", options.path, "The path to the new package; if not specified will initialize the current directory");

		cmd.callback([]() {
			if(options.path.empty()) {
				if(std::filesystem::exists("package.toml")) {
					throw CommandError{"new cannot be run on existing packages"};
				}

				createConfig();

				if(!std::filesystem::exists("src")) {
					createSrc();
				}else {
					if(!std::filesystem::exists("src/main.cpp")) {
						createMain();
					}
				}
			}else {
				if(std::filesystem::exists(options.path)) {
					throw CommandError{'"' + std::filesystem::absolute(options.path).native() + '"' + " already exists"};
				}

				std::filesystem::create_directories(options.path);

				std::filesystem::current_path(options.path);

				createConfig();
				createSrc();
			}
		});
	}
};
