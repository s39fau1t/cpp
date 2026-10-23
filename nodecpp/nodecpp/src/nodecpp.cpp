#include <nodecpp/nodecpp.h>

#include <uv.h>

namespace nodecpp {
	Error::Error(ErrorCode code) : code(code) {
		message = uv_strerror(static_cast<int>(code));
		name = uv_err_name(static_cast<int>(code));
	}

	const char* Error::what() const noexcept {
		return message;
	}

	void run() {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);

		uv_loop_close(uv_default_loop());
	}
}
