/*
module;

#include <string>
#include <filesystem>
#include <fstream>
#include <functional>

#include <CLI11.hpp>
#include <rang.hpp>

export module commands.init;

import commands.error;

//TODO
struct Options {
	bool quiet = false;
	std::string name = std::filesystem::current_path().filename();

	std::string path = ".";
};

void createConfig(const std::string& path, const std::string& name) {
		std::ofstream file(path);

		//TODO: Other properties
		file << R"([package]
name = ")" << name << R"("

[dependencies])";
}

void createMain(const std::string& path) {
		std::ofstream file(path);

		file << R"(#include <iostream>

int main() {
	std::cout << "Hello, World!\n";
})";
}

void createSrc(const std::string& path) {
	std::filesystem::create_directory(path);

	createMain(path + "/main.cpp");
}

export namespace init {
	void add(CLI::App& app) {
		static Options options;

		CLI::App& cmd = *app.add_subcommand("init", "Create a new package in an existing directory");

		cmd.add_flag("-q,--quiet", options.quiet, "Do not print log messages");
		cmd.add_option("--name", options.name, "Set the resulting package name, defaults to the directory name");
		//TODO: Help message
		cmd.add_option("path", options.path, "The directory to create a new package in (default: .)");

		//TODO: Templates? e.g. binary, library
		cmd.callback([] {
			if(!std::filesystem::exists(options.path)) {
				std::filesystem::create_directory(options.path);

				createConfig(options.path + "/package.toml", options.name);
				createSrc(options.path + "/src");
			}else {
				const std::string configPath = options.path + "/package.toml";

				if(std::filesystem::exists(configPath)) {
					throw CommandError("init cannot be run on an existing package");
				}

				createConfig(configPath, options.name);

				const std::string srcPath = options.path + "/src";

				if(std::filesystem::exists(srcPath)) {
					const std::string mainPath = srcPath + "/main.cpp";

					if(!std::filesystem::exists(mainPath)) {
						createMain(mainPath);
					}
				}else {
					createSrc(srcPath);
				}
			}

			if(!options.quiet) {
				std::cout << rang::fg::green << "Created " << rang::fg::reset << "binary (application) package\n";
			}
		});
	}
};
*/
