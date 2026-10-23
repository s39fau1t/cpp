module;

#include <string_view>
#include <string>
#include <cstdio>
#include <concepts>
#include <type_traits>
#include <algorithm>
#include <array>
#include <filesystem>
#include <memory>
#include <format>
#include <utility>
#include <span>

#include <iostream>

#include <unistd.h>

#include <json.hpp>

module commands:compile;

import :error;

using json = nlohmann::json;

constexpr auto size = std::size_t{256};

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITCODE
#endif

struct CommandResult {
	int status;
	std::string output;
};

//NOTE: command must be null-terminated
//TODO: static method of Compiler::Command?
CommandResult execute(std::string_view command) {
	//TODO: is FILE& okay?
	auto& pipe = *popen(command.data(), "r");

	auto output = std::string{};
	auto read = std::size_t{};

	//TODO: this loop could be nicer (for loop?)
	while(true) {
		const auto end = output.size();

		output.resize(end + size);

		read = std::fread(&output[end], sizeof(std::string::value_type), size, &pipe);

		if(read != size) {
			output.resize(output.size() - (size - read));

			break;
		}
	}

	//std::cout << output << '\n';

	const auto status = pclose(&pipe);

	return {WEXITSTATUS(status), std::move(output)};
}

//TODO: this is trash
/*
std::optional<std::filesystem::path> findProgram(std::string_view program) {
	auto result = execute(std::format("command -v {}", program));

	if(result.status != 0) {
		return {};
	}

	//TODO: Do I need this?
	return {std::move(result.output)};
}
*/

//IDEA: polymorphism?
//friend funciton getCompiler()?
//returns unique_ptr to Compiler
//or could be static?
//but then I would have to foreward declare compiler and its derived classes?

template <typename T>
struct IsStdArrayImpl : std::false_type {};

template <typename T, std::size_t N>
struct IsStdArrayImpl<std::array<T, N>> : std::true_type {};

template <typename T>
using IsStdArray = IsStdArrayImpl<std::remove_cvref_t<T>>;

template <typename T>
concept StdArray = IsStdArray<T>::value;

template <std::size_t N>
struct TemplateStr {
	char value[N];

	consteval TemplateStr(const char (&value)[N]) {
		std::copy_n(value, N, this->value);
	}
};

export class Compiler {
	public:
		/*
		struct Option {
			enum class Type {
				Output,
				NoLink
			};

			Type type;
			std::string value;
		};
		*/

		enum class FileType {
			Module,
			//TODO: better name?
			Source
		};

		enum class Option {
			FileType,
			//TODO: seperate func for files? (should that include FileType?)
			File,
			Output,
			NoLink,
			//TODO: always c++20?
			Std,
			ModulePath,
			ModuleOutput,
			Precompile
		};

		//TODO: How to get output
		template <typename T>
		class Command {
			public:
				//TODO: Seperate func for flags?
				virtual Command& addOption(Option option, std::string value = "") = 0;

				Command& end() {
					command += " 2>&1";

					return *this;
				}
				//TODO
				//virtual?
				virtual T execute() = 0;

				virtual ~Command() = default;

			protected:
				std::string command;

				Command(std::string command) : command(std::move(command)) {}
		};

		template <typename T>
			requires std::derived_from<T, Compiler> && requires {
				//TODO: Check for constructor of array of std::filesystem::path's
				//T{std::filesystem::path{}};

				//TODO: Enforce type of array
				//TODO: also enforce constexpr?
				{T::bins} -> StdArray;

				T{std::array<std::filesystem::path, T::bins.size()>{}};
			}
		static void check(std::unique_ptr<Compiler>& compiler) {
			if(compiler) {
				return;
			}

			auto paths = std::array<std::filesystem::path, T::bins.size()>{};

			for(auto i = typename decltype(T::bins)::size_type{0}; i < T::bins.size(); i ++) {
				auto result = execute(std::format("command -v {}", T::bins[i]));

				if(result.status != 0) {
					return;
				}

				//Remove trailing newline
				result.output.pop_back();

				paths[i] = std::move(result.output);
			}

			compiler = std::make_unique<T>(std::move(paths));
		}

		//TODO: Find compiler
		//https://cmake.org/pipermail/cmake/2013-March/053819.html

		//TODO: json?
		/*
		virtual json scan(const std::filesystem::path& file, std::span<Option> options) = 0;
		virtual void compile(const std::filesystem::path& file, std::span<Option> options) = 0;
		*/
		virtual std::unique_ptr<Command<json>> scan() = 0;
		virtual std::unique_ptr<Command<void>> compile() = 0;

		virtual ~Compiler() = default;
};

class Clang : public Compiler {
	public:
		template <typename T>
		class Command : public Compiler::Command<T> {
			//TODO: ok?
			using Compiler::Command<T>::Command;

			public:
				Command& addOption(Option option, std::string value) override {
					auto prefix = std::string_view{};

					switch(option) {
						using enum Compiler::Option;

						case FileType:
							prefix = "-x";
							break;
						case File:
							break;
						case Output:
							prefix = "-o ";
							break;
						case NoLink:
							prefix = "-c";
							break;
						case Std:
							prefix = "-std=";
							break;
						case ModulePath:
							prefix = "-fprebuilt-module-path=";
							break;
						case ModuleOutput:
							if(value.empty()) {
								prefix = "-fmodule-output";
							}else {
								prefix = "-fmodule-output=";
							}
							break;
						case Precompile:
							prefix = "--precompile";
							break;
					}

					this->command.reserve(this->command.size() + 1 + prefix.size() + value.size());

					this->command += ' ';
					this->command += prefix;
					this->command += value;

					return *this;
				}
		};

		class Scan : public Command<json> {
			public:
				//TODO
				//TODO: this is really stupid
				Scan(const std::filesystem::path& scanner, const std::filesystem::path& compiler) : Command{std::format("{} -format=p1689 -- {}", scanner.string(), compiler.string())} {}

				json execute() override {
					auto result = ::execute(command);

					if(result.status != 0) {
						auto errorMessage = std::string_view{result.output};

						errorMessage.remove_prefix(errorMessage.find('\n') + 1);
						errorMessage.remove_suffix(errorMessage.size() - errorMessage.find('\n'));

						//TODO: get rid of trailing newline?
						//TODO: save file????
						throw CommandError{std::format("failed to scan file \"{}\":\n{}", file.string(), errorMessage)};
					}

					return json::parse(result.output);
				}
			private:
				std::filesystem::path file;
		};

		class Compile : public Command<void> {
			public:
				//TODO
				Compile(const std::filesystem::path& compiler) : Command{compiler.string()} {}

				void execute() override {
					auto result = ::execute(command);

					if(result.status != 0) {
						throw CommandError{result.output};
					}
				}
		};

		static constexpr auto bins = std::array<std::string_view, 2>{"clang++", "clang-scan-deps"};

		Clang(std::array<std::filesystem::path, 2> paths) : compiler{std::move(paths[0])}, scanner{std::move(paths[1])} {}

		std::unique_ptr<Compiler::Command<json>> scan() override {
			return std::make_unique<Scan>(scanner, compiler);
		}
		std::unique_ptr<Compiler::Command<void>> compile() override {
			return std::make_unique<Compile>(compiler);
		}

		//TODO: should file be part of Options?
		//TODO: I still need a better way of passing options
		/*
		json scan(const std::filesystem::path& file, std::span<Compiler::Option> options) override {
			//TODO: Function for this
			auto command = std::format("{} -format=p1689 -- ", scanner.string());

			getCommand(command, file, options);

			auto result = execute(command);

			//TODO: I think calling string() over and over again is inefficient
			//TODO: these options probably should not be hardcoded
			//auto result = execute(std::format("{} -format=p1689 -- {} {} {} 2>&1", scanner.string(), compiler.string(), options, file.string()));

			if(result.status != 0) {
				auto errorMessage = std::string_view{result.output};

				errorMessage.remove_prefix(errorMessage.find('\n') + 1);
				errorMessage.remove_suffix(errorMessage.size() - errorMessage.find('\n'));

				throw CommandError{std::format("failed to scan file \"{}\":\n{}", file.string(), errorMessage)};
			}

			return json::parse(std::move(result.output));
		}
		//TODO
		//TODO: specify where to put the compiled file
		//TODO: compileModule func?
		void compile(const std::filesystem::path& file, std::span<Compiler::Option> options) override {
			auto command = std::string{};
			getCommand(command, file, options);

			auto result = execute(command);
		}
		*/
	private:
		std::filesystem::path compiler;
		std::filesystem::path scanner;

		/*
		std::string_view optionPrefix(Compiler::Option::Type type) {
			using enum Compiler::Option::Type;

			switch(type) {
				case Output: return "-o ";
				case NoLink: return "-c";
			}
		}

		//TODO: this kinda sucks
		void getCommand(std::string& command, const std::filesystem::path& file, std::span<Compiler::Option> options) {
			std::string compilerStr = compiler.string();

			command.reserve(command.size() + compilerStr.size() + 1);

			command += compilerStr;
			command += ' ';

			for(auto& option : options) {
				auto prefix = optionPrefix(option.type);

				command.reserve(command.size() + prefix.size() + option.value.size() + 1);

				command += prefix;
				command += option.value;
				command += ' ';
			}

			auto fileStr = file.string();

			command.reserve(command.size() + fileStr.size() + 5);

			command += fileStr;
			command += " 2>&1";
		}
		*/
};

export std::unique_ptr<Compiler> getCompiler() {
	auto compiler = std::unique_ptr<Compiler>{};

	Compiler::check<Clang>(compiler);

	return compiler;
}

/*
export json scan(const std::filesystem::path& file) {
	//TODO: which clang++ is cheating; actually find compiler like cmake does
	//TODO: Is native() okay? maybe string()?
	return json::parse(execute(std::format("clang-scan-deps -format=p1689 -- $(which clang++) {} -o {}", file.native(), file.filename().replace_extension(".o").native())));

}
*/
