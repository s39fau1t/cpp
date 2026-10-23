module;

#include <stdexcept>
#include <filesystem>
#include <string_view>
#include <format>
#include <iterator>
#include <iostream>

#include <toml.hpp>
#include <rang.hpp>

export module commands:error;

export class CommandError : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

//TODO: Define this in config.cpp? Is that the only file that needs it?
export class ConfigError : public CommandError {
	public:
		//TODO: native() or string()
		ConfigError(const std::filesystem::path& path, std::string_view description) : CommandError{std::format("failed to parse config file at \"{}\":\n{}", path.native(), description)} {}
};

export class MissingFieldError : public ConfigError {
	public:
		MissingFieldError(const std::filesystem::path& path, std::string_view field) : ConfigError{path, std::format("Missing field \"{}\"", field)} {}
		MissingFieldError(const std::filesystem::path& path, std::string_view field, std::string_view parent) : ConfigError{path, std::format("Missing field \"{}\" in \"{}\"", field, parent)} {}
};

//TODO: Should this go here?
//TODO: string_view or const char* (maybe string but probably not?)
constexpr std::string_view nodeTypeName(toml::node_type type) {
	switch(type) {
		case toml::node_type::table: return "table";
		case toml::node_type::array: return "array";
		case toml::node_type::string: return "string";
		case toml::node_type::boolean: return "boolean";
		case toml::node_type::integer: return "integer";
		case toml::node_type::floating_point: return "floating point";
		case toml::node_type::date: return "date";
		case toml::node_type::time: return "time";
		case toml::node_type::date_time: return "date time";

		default: throw std::invalid_argument("invalid node type");
	}
}

//TODO: Coloring: types, fields (no quotes?)
export class WrongTypeError : public ConfigError {
	public:
		WrongTypeError(const std::filesystem::path& path, std::string_view field, toml::node_type type, toml::node_type expectedType) : ConfigError{path, std::format("invalid type {} for field \"{}\", expected {}", nodeTypeName(type), field, nodeTypeName(expectedType))} {}
};

export void warn(std::string_view message) {
	//TODO: std::formatter specializations do not yet work in clang
	//std::format_to(std::ostream_iterator<char>{std::cerr}, "{}warning:{} {}", rang::fg::magenta, rang::fg::reset, message);
	std::cerr << rang::style::bold << rang::fg::magenta << "warning:" << rang::fg::reset << ' ' << message << rang::style::reset << '\n';
}
