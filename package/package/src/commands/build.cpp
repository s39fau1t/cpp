module;

#include <filesystem>
#include <vector>
#include <format>
#include <unordered_map>
#include <memory>

#include <algorithm>


#include <iostream>

#include <CLI11.hpp>
#include <json.hpp>

export module commands:build;

import :config;
import :error;

import :compile;

using json = nlohmann::json;

namespace build {
	struct Options {};

	struct Module {
		std::filesystem::path path;
		json::array_t deps;
		bool compiled = false;
	};

	void getSources(std::vector<std::filesystem::path>& sources, const std::filesystem::path& dir) {
		for(const auto& path : std::filesystem::directory_iterator(dir)) {
			if(path.is_directory()) {
				getSources(sources, path);

				continue;
			}

			if(path.path().extension() == ".cpp") {
				sources.push_back(path);
			}else {
				//TODO: change once there's a formatter for std::filesystem::path
				warn(std::format("non-source file \"{}\" in src directory", path.path().lexically_relative(dir).string()));
			}
		}
	}

	//TODO: return true (or false) if any deps needed to be recompiled
	bool compileDeps(Compiler& compiler, std::unordered_map<std::string, Module>& modules, const json::array_t& deps) {
		bool recompile = false;

		for(const auto& requirement : deps) {
			//TODO: throw on missing module

			auto name = std::string{requirement["logical-name"]};

			auto& mod = modules[name];

			//TODO: combine?
			if(mod.compiled) {
				continue;
			}

			if(!compileDeps(compiler, modules, mod.deps)) {
				continue;
			}

			recompile = true;

			std::replace(name.begin(), name.end(), ':', '-');

			compiler.compile()
				->addOption(Compiler::Option::FileType, "c++-module")
				.addOption(Compiler::Option::File, mod.path)
				//.addOption(Compiler::Option::Precompile)
				.addOption(Compiler::Option::Std, "c++20")
				.addOption(Compiler::Option::ModulePath, "build/bmi")
				.addOption(Compiler::Option::ModuleOutput, std::format("build/bmi/{}.pcm", name))
				.addOption(Compiler::Option::NoLink)
				.addOption(Compiler::Option::Output, std::format("build/obj/{}.o", name))
				//.addOption(Compiler::Option::Output, std::format("build/{}.pcm", name))
				.end()
				.execute();
			mod.compiled = true;
		}

		return recompile;
	}

	export void add(CLI::App& app) {
		static auto options = Options{};

		auto& cmd = *app.add_subcommand("build", "Compile the current package");
		cmd.alias("b");

		cmd.callback([] {
			const auto config = Config{};

			auto compiler = getCompiler();

			auto src = config.path;
			src.replace_filename("src");

			auto sources = std::vector<std::filesystem::path>{};
			getSources(sources, src);

			auto modules = std::unordered_map<std::string, Module>{};

			auto mainDeps = json{};

			for(auto& file : sources) {
				json deps = compiler->scan()
					->addOption(Compiler::Option::File, file)
					.addOption(Compiler::Option::Std, "c++20")
					.end()
					.execute();

				if(file.filename() == "main.cpp") {
					mainDeps = std::move(deps);

					continue;
				}

				auto& rules = deps["rules"][0];

				auto& provides = rules["provides"];

				if(provides.is_null()) {
					//TODO: same as todo in getSources
					warn(std::format("non-module source file \"{}\" other than main.cpp in src directory", file.lexically_relative(src).string()));

					continue;
				}

				modules.emplace(std::move(provides[0]["logical-name"]), Module{std::move(file), rules.value("requires", json::array_t{})});
			}

			//TODO: check that main is not a module

			std::filesystem::create_directory("build");
			std::filesystem::create_directory("build/bmi");
			std::filesystem::create_directory("build/obj");

			compileDeps(*compiler, modules, mainDeps["rules"][0].value("requires", json::array_t{}));

			compiler->compile()
				->addOption(Compiler::Option::File, "src/main.cpp")
				.addOption(Compiler::Option::Std, "c++20")
				.addOption(Compiler::Option::ModulePath, "build/bmi")
				.addOption(Compiler::Option::Output, "build/obj/main.o")
				.addOption(Compiler::Option::NoLink)
				.end()
				.execute();

			auto link = compiler->compile();

			link->addOption(Compiler::Option::File, "build/obj/main.o");

			for(const auto& [name, mod] : modules) {
				if(!mod.compiled) {
					//TODO: same as todo above warn above
					warn(std::format("unused module \"{}\" in file \"{}\" in src directory", name, mod.path.lexically_relative(src).string()));

					continue;
				}

				link->addOption(Compiler::Option::File, std::format("build/obj/{}.o", name));
			}

			link->addOption(Compiler::Option::Output, "build/main")
				.end()
				.execute();

			//FIXME: this is not complete; it should link and to do that it needs to compile dependant bmi's (should it save them somwhere?)


			/*
			 * FILE STRUCTURE:
			 * build/
			 * 	main (executable)
			 * 	bmi/ (*.pcm files)
			 * 	obj/ (*.o files)
			 *
			 * planned:
			 * build/debug/ and build/release/
			 * deps/ for dependencies
			 * compile_commands.json
			*/

			/*

			auto mainIt = deps.find("main.cpp");

			if(mainIt == deps.end()) {
				throw CommandError{"TODO"};
			}

			deps.erase(mainIt);

			compileDeps(compiler, deps, mainIt->second);
			*/

			//TODO: Should main be the default target?

			/*
			std::vector<std::filesystem::path> sources;
			//TODO: Check for sources directory (should I do that in config constructor?)
			getSources(sources, config.path.parent_path() / "src");

			for(const std::filesystem::path& file : sources) {
				std::cout << scan(file).dump(4) << '\n';
			}
			*/

			/*
			//TODO: Is there a more efficient way to do this? i.e. in getSources?
			auto main = std::find_if(sources.begin(), sources.end(), [](const std::filesystem::path& path) {
				return path.filename() == "main.cpp";
			});
			//TODO: throw on no main
			*/
		});
	}
};
