module;

#include <filesystem>
#include <string_view>
#include <fstream>
#include <limits>
#include <string>
#include <format>

#include <toml.hpp>

export module commands:getConfig;

import :error;

export std::filesystem::path findConfig() {
	auto path = std::filesystem::current_path();

	const auto root = path.root_path();

	while(true) {
		const auto configPath = path / "package.toml";

		if(std::filesystem::exists(configPath)) {
			return configPath;
		}

		if(path == root) {
			break;
		}

		path = path.parent_path();
	}

	throw CommandError{"could not find \"package.toml\" in \"" + std::filesystem::current_path().string() + "\" or any parent directories"};
}

export toml::table getConfig(std::string_view path) {
	try {
		return toml::parse_file(path);
	}catch(const toml::parse_error& error) {
		auto file = std::ifstream{path};

		for(auto i = 0; i < error.source().begin.line - 1; i ++) {
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		auto line = std::string{};
		std::getline(file, line);

		auto arrow = std::string{};
		arrow.reserve(error.source().begin.column);

		arrow.append(error.source().begin.column - 1, ' ');
		arrow.push_back('^');

		throw ConfigError{path, std::format("  |\n{} | {}\n  | {}\n{}", error.source().begin.line, line, arrow, error.description())};
	}
}
export toml::table getConfig() {
	return getConfig(findConfig().native());
}
