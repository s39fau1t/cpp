#include "promise.h"

namespace koa {
	Promise::Promise(std::coroutine_handle<Promise::promise_type> handle) : handle(handle) {}

	Promise Promise::promise_type::get_return_object() {return std::coroutine_handle<Promise::promise_type>::from_promise(*this);}
	std::suspend_never Promise::promise_type::initial_suspend() {return {};}
	std::suspend_never Promise::promise_type::final_suspend() noexcept {return {};}
	void Promise::promise_type::return_void() {}
	void Promise::promise_type::unhandled_exception() {}
}
