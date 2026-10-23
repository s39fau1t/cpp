#pragma once

#include <coroutine>

//TODO: template?
namespace koa {
	struct Promise {
		struct promise_type {
			Promise get_return_object();
			std::suspend_never initial_suspend();
			std::suspend_never final_suspend() noexcept;
			void return_void();
			void unhandled_exception();
		};

		Promise(std::coroutine_handle<promise_type> handle);

		bool await_ready();

	private:
		std::coroutine_handle<promise_type> handle;
	};
};

//TODO: remove test
koa::Promise test() {
	co_return;
}

koa::Promise test2() {
	co_await test();

	co_return;
}
