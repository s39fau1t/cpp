#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace koa {
	class Response {
	public:
		//TODO: const?
		std::unordered_map<std::string, std::string> header;
		std::unordered_map<std::string, std::string>& headers;

		//TODO: no socket?

		//TODO: Table for default messages
		int status = 404;
		std::string message;

		//TODO: length? redundant

		std::variant</*TODO: Do I need monostate, or just empty string?*/std::monostate, std::string /*TODO: buffer*/ /*TODO: stream*/ /*TODO: probably don't allow object*/ > body;

		//TODO: should I include redundant get, has, set, append, remove?
		//TODO: Return a reference or const-reference?
		//std::string get(const std::string& key);

		//TODO: type? it is also redundant
		//TODO: is

		void redirect(const std::string& url, std::string alt = "/");

		//TODO: attachment

		//TODO: headerSend

		//TODO: lastModified? redundant

		//TODO: etag? maybe redundant

		//TODO: vary? maybe redundant

		//TODO: flushHeaders
	};
}
