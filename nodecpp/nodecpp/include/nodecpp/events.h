#pragma once

#include <cstddef>
#include <algorithm>
#include <concepts>
#include <ios>
#include <type_traits>
#include <unordered_map>
#include <string_view>
#include <variant>
#include <deque>
#include <functional>
#include <utility>
#include <vector>

//TODO: Clean up templates with better concepts

//TODO: Do I need to copy?
template <std::size_t size>
struct EventName {
	char value[size];

	consteval EventName(const char (&str)[size]) {
		std::copy_n(str, size, value);
	}

	template <std::size_t thatSize>
	consteval bool operator==(const EventName<thatSize>& that) const {
		if constexpr(size != thatSize) {
			return false;
		}else {
			return std::equal(value, value + size, that.value);
		}
	}
};

template <EventName _name, typename... Args>
struct Event {
	using FuncType = void(Args...);

	inline static constexpr EventName name = _name;
};

template <typename T>
concept EventType = requires {
	std::function<typename T::FuncType>();

	{T::name.value} -> std::convertible_to<const char (&)[]>;
};

//Couldn't find a way to implement the other overload of listenerCount with current implementation
//That means removeListener also can't work (and also off, an alias)
//TODO: I could use std::shared_ptr or something to implement the other methods but I don't like the idea of allocating

//TODO: I should have some usings (aliases) to make this code cleaner

/*
 * A note on listeners()
 * The listeners() method in nodecpp is more like the rawListeners() method in nodejs in that it returns the raw internal array
 * There is no equivalent for nodejs' listeners() because the everything in the internal array is the same type (unlike node, in which some will be functions and some will be OnceWrappers), making a listeners() method that makes everything functions useless (you can just use the .listener property of Listener)
*/

//TODO: newListener and removeListener events
//TODO: Should I even implement those? They would have to pass in a string_view and std::variant... Are they even that useful?

//TODO: Find a way to put public interface first
template <EventType... Events>
class EventEmitter {
	struct LookupFailed {};

	template <EventName key, EventType Event, EventType... Es>
	struct LookupImpl {
		using Type = typename std::conditional_t<key == Event::name, Event, typename LookupImpl<key, Es...>::Type>;
	};

	template <EventName key, EventType Event>
	struct LookupImpl<key, Event> {
		using Type = typename std::conditional_t<key == Event::name, Event, LookupFailed>;
	};

	template <EventName key, EventType... Es>
	using Lookup = typename LookupImpl<key, Es...>::Type;

	template <typename FuncType>
	class Listener;

	template <typename... Args>
	class Listener<void(Args...)> {
		using ListenerType = std::function<void(Args...)>;

		using ListenersType = std::deque<Listener<void(Args...)>>;

	public:
		ListenerType listener;

		bool once;

		bool fired = false;

		Listener(ListenerType listener, bool once, ListenersType& listeners) : listener(std::move(listener)), once(once), listeners(&listeners) {}

		void operator()(Args&&... args) {
			if(once) {
				if(fired) {
					return;
				}

				listeners->erase(std::find_if(listeners->begin(), listeners->end(), [this](Listener<void(Args...)>& that) {
					return this == &that;
				}));

				fired = true;
			}

			listener(std::forward<Args>(args)...);
		}

	private:
		ListenersType* listeners;
	};

	template <typename FuncType>
	using Listeners = std::deque<Listener<FuncType>>;

	template <EventType T, EventType... Ts>
	class Map {
		template <EventName eventName>
		using GetType = Listeners<typename Lookup<eventName, T, Ts...>::FuncType>;

		using ValueType = Listeners<typename T::FuncType>;

	public:
		template <EventName eventName>
		GetType<eventName>& get() {
			if constexpr(T::name == eventName) {
				return value;
			}else {
				return next.template get<eventName>();
			}
		}

		template <EventName eventName>
		const GetType<eventName>& get() const {
			if constexpr(T::name == eventName) {
				return value;
			}else {
				return next.template get<eventName>();
			}
		}

	private:
		ValueType value;

		Map<Ts...> next;
	};

	template <EventType T>
	class Map<T> {
		using ValueType = Listeners<typename T::FuncType>;

	public:
		template <EventName eventName>
		ValueType& get() {
			return value;
		}

		template <EventName eventName>
		const ValueType& get() const {
			return value;
		}

	private:
		ValueType value;
	};

	Map<Events...> listenersMap;

public:
	template <EventName eventName>
	EventEmitter& on(std::function<typename Lookup<eventName, Events...>::FuncType> listener) {
		listenersMap.template get<eventName>().emplace_back(std::move(listener), false, listenersMap.template get<eventName>());

		return *this;
	}

	template <EventName eventName>
	EventEmitter& once(std::function<typename Lookup<eventName, Events...>::FuncType> listener) {
		listenersMap.template get<eventName>().emplace_back(std::move(listener), true, listenersMap.template get<eventName>());

		return *this;
	}

	template <EventName eventName>
	EventEmitter& prependListener(std::function<typename Lookup<eventName, Events...>::FuncType> listener) {
		listenersMap.template get<eventName>().emplace_front(std::move(listener), false, listenersMap.template get<eventName>());

		return *this;
	}

	template <EventName eventName>
	EventEmitter& prependOnceListener(std::function<typename Lookup<eventName, Events...>::FuncType> listener) {
		listenersMap.template get<eventName>().emplace_front(std::move(listener), true, listenersMap.template get<eventName>());

		return *this;
	}

	template <EventName eventName>
	EventEmitter& addListener(std::function<typename Lookup<eventName, Events...>::FuncType> listener) {
		return on<eventName>(std::move(listener));
	}

	EventEmitter& removeAllListeners() {
		(listenersMap.template get<Events::name>().clear(), ...);

		return *this;
	}

	template <EventName eventName>
	EventEmitter& removeAllListeners() {
		listenersMap.template get<eventName>().clear();

		return *this;
	}

	template <EventName eventName, typename... Args>
	bool emit(Args&&... args) {
		using FuncType = typename Lookup<eventName, Events...>::FuncType;

		Listeners<FuncType>& funcs = listenersMap.template get<eventName>();

		if(funcs.size() == 0) {
			return false;
		}

		for(typename Listeners<FuncType>::size_type i = 0; i < funcs.size();) {
			bool once = funcs[i].once;

			funcs[i](std::forward<Args>(args)...);

			if(!once) {
				i ++;
			}
		}

		return true;
	}

	std::vector<std::string_view> eventNames() const {
		std::vector<std::string_view> names;

		([this, &names] {
			if(listenersMap.template get<Events::name>().size() > 0) {
				names.push_back(Events::name.value);
			}
		}(), ...);

		return names;
	}

	//TODO: Max listeners

	template <EventName eventName>
	//TODO: Clean up this declaration?
	typename Listeners<typename Lookup<eventName, Events...>::FuncType>::size_type listenerCount() const {
		return listenersMap.template get<eventName>().size();
	}

	template <EventName eventName>
	Listeners<typename Lookup<eventName, Events...>::FuncType>& listeners() const {
		return listenersMap.template get<eventName>();
	}
};
