#include <iostream>
#include <string_view>

#include <uv.h>

void printError(const char* prefix, int code) {
	std::cerr << prefix << ": " << uv_strerror(code) << '\n';
}

void closeAndDeallocate(uv_handle_t* handle) {
	uv_close(handle, [](uv_handle_t* handle) {
		delete handle;
	});
}

uv_loop_t* loop = uv_default_loop();

int main() {
	uv_tcp_t server;
	uv_tcp_init(loop, &server);

	sockaddr_in addr;
	uv_ip4_addr(nullptr, 8000, &addr);

	uv_tcp_bind(&server, reinterpret_cast<sockaddr*>(&addr), 0);

	uv_listen(reinterpret_cast<uv_stream_t*>(&server), 10, [](uv_stream_t* server, int status) {
		if(status < 0) {
			printError("Connection error", status);

			return;
		}

		uv_tcp_t* client = new uv_tcp_t;
		uv_tcp_init(loop, client);

		if(uv_accept(server, reinterpret_cast<uv_stream_t*>(client)) == 0) {
			std::cout << "Connection!\n";

			uv_read_start(reinterpret_cast<uv_stream_t*>(client), [](uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf) {
				buf->base = new char[suggestedSize];
				buf->len = suggestedSize;
			}, [](uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
				if(nread == UV_EOF) {
					std::cout << "Disconnection!\n";

					//Client disconnected
					closeAndDeallocate(reinterpret_cast<uv_handle_t*>(client));

					return;
				}else if(nread < 0) {
					//error
					printError("Socket read error", nread);

					closeAndDeallocate(reinterpret_cast<uv_handle_t*>(client));

					return;
				}

				std::cout << "Recieved:\n" << std::string_view(buf->base, nread);

				//echo
				uv_buf_t writeBuf;
				writeBuf.base = buf->base;
				writeBuf.len = nread;

				uv_write_t writeReq;
				writeReq.data = writeBuf.base;

				uv_write(&writeReq, client, &writeBuf, 1, [](uv_write_t* req, int status) {
					if(status < 0) {
						printError("Socket write error", status);

						return;
					}

					delete reinterpret_cast<char*>(req->data);
				});
			});
		}else {
			closeAndDeallocate(reinterpret_cast<uv_handle_t*>(client));
		}
	});

	uv_run(loop, UV_RUN_DEFAULT);

	uv_close(reinterpret_cast<uv_handle_t*>(&server), nullptr);

	uv_loop_close(loop);
}
