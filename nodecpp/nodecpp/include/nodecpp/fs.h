#pragma once

//TODO: Callback API first
//TODO: I think I can do synchronous by just passing nullptr to uv funcs

#include <string_view>
#include <functional>
#include <optional>

#include <uv.h>

//TODO: Is this how I should include this?
#include <nodecpp/nodecpp.h>

//TODO: Do I have to undef everything?
#undef O_RDONLY
#undef O_WRONLY
#undef O_RDWR

namespace fs {
	//TODO: Other constants
	//TODO: Can I do this?
	namespace constants {
		constexpr int O_RDONLY = 0x0000;
		constexpr int O_WRONLY = 0x0001;
		constexpr int O_RDWR = 0x0002;
	};

	void open(std::string_view path, int flags, int mode, std::function<void(std::optional<nodecpp::Error>, int)> cb);
	void open(std::string_view path, int flags, std::function<void(std::optional<nodecpp::Error>, int)> cb);
	//TODO:                          string may be inefficient; maybe enum?
	void open(std::string_view path, std::string_view flags, std::function<void(std::optional<nodecpp::Error>, int)> cb);
	void open(std::string_view path, std::function<void(std::optional<nodecpp::Error>, int)> cb);
};
