#include <iostream>

#include <CLI11.hpp>
#include <rang.hpp>

#include <format>
#include <string_view>

import commands;

int main() {
	auto app = CLI::App{"Unnamed C++ package manager", "package"};

	//TODO: This behavior is more like cargo, but is that good?
	app.require_subcommand(0, 1);

	newCmd::add(app);
	build::add(app);

	/*
	build::add(app);
	init::add(app);
	*/

	/*
	app.add_subcommand("build", "Compile the current package")->alias("b");
	//No check, I think
	app.add_subcommand("clean", "Remove the target directory");
	//Doc?
	app.add_subcommand("new", "Create a new package");
	app.add_subcommand("init", "Create a new package in an existing directory");
	app.add_subcommand("add", "Add dependencies to a manifest file");
	app.add_subcommand("remove", "Remove dependencies to a manifest file");
	app.add_subcommand("run", "Run a binary or example of the local package")->alias("r");
	//Test?
	//Bench?
	//Update
	//Search
	//Publish
	//Install
	//Uninstall
	//More?
	*/

	if(CLI::argc() == 1) {
		app.get_help_ptr()->force_callback();
	}

	try {
		app.parse();
	}catch(const CLI::ParseError &error) {
		if(error.get_name() != "CallForHelp" && error.get_name() != "CallForAllHelp" && error.get_name() != "CallForVersion") {
			std::cerr << rang::fg::red << "error: " << rang::fg::reset;
		}

		return app.exit(error);
	}catch(const CommandError& error) {
		std::cerr << rang::fg::red << "error: " << rang::fg::reset << error.what() << '\n';

		return -1;
	}
}
