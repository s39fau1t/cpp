#pragma once

#include <vector>
#include <functional>

#include "context.h"
#include "request.h"
#include "response.h"
#include "promise.h"

namespace koa {
	using Callback = const std::function<Promise()>&;

	//TODO: Should this be called Application?
	class Koa {
	public:
	};
};
