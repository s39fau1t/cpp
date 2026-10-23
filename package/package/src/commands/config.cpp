module;

#include <filesystem>
#include <string_view>
#include <string>
#include <stdexcept>

#include <toml.hpp>
#include <semver.hpp>

module commands:config;

//TODO: Move getConfig functions inline
import :getConfig;
import :error;

//TODO: Define ConfigError here?

//TODO: Rename
template <typename T>
T& getNode(toml::table& tbl, std::string_view key) {
	auto node = tbl[key];

	if(!node) {
		throw MissingFieldError{*tbl.source().path, key};
	}

	if(!node.is<T>()) {
		throw WrongTypeError{*tbl.source().path, key, node.type(), T{}.type()};
	}

	return *node.as<T>();
}

export struct Config {
	struct Package {
		std::string name;
		semver::version version;
	};

	struct Binary {
		std::string path;
	};

	std::filesystem::path path;

	Package package;

	Config() {
		path = findConfig();

		auto tbl = getConfig(path.native());

		auto& packageToml = getNode<toml::table>(tbl, "package");

		try {
			package = {*getNode<toml::value<std::string>>(packageToml, "name"), semver::version(*getNode<toml::value<std::string>>(packageToml, "version"))};
		}catch(const std::invalid_argument& error) {
			throw ConfigError{path, "invalid version string; see https://semver.org/ for format"};
		}


		//TODO: dependencies, etc
	}
};
