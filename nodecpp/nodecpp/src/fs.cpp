#include <nodecpp/fs.h>

#include <utility>

#include <uv.h>

#include <nodecpp/nodecpp.h>

namespace fs {
	void open(std::string_view path, int flags, int mode, std::function<void(std::optional<nodecpp::Error>, int)> cb) {
		uv_fs_t* req = new uv_fs_t;
		req->data = new std::function<void(std::optional<nodecpp::Error>, int)>(std::move(cb));

		uv_fs_open(uv_default_loop(), req, path.data(), flags, mode, [](uv_fs_t* req) {
				std::function<void(std::optional<nodecpp::Error>, int)>& cb = *static_cast<std::function<void(std::optional<nodecpp::Error>, int)>*>(req->data);

			if(req->result >= 0) {
				cb({}, req->result);
			}else {
				cb(static_cast<nodecpp::ErrorCode>(req->result), -1);
			}

			delete &cb;
			delete req;
		});
	}
	void open(std::string_view path, int flags, std::function<void(std::optional<nodecpp::Error>, int)> cb) {
		open(path, flags, 0666, std::move(cb));
	}
	//TODO
	void open(std::string_view path, std::string_view flags, std::function<void(std::optional<nodecpp::Error>, int)> cb) {
	}
	void open(std::string_view path, std::function<void(std::optional<nodecpp::Error>, int)> cb) {
		open(path, constants::O_RDONLY, std::move(cb));
	}
}
