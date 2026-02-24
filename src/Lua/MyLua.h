#pragma once

#define SOL_NO_EXCEPTIONS 1
#include "sol/sol.hpp"

#include "Log/Log.h"

#include <cxxabi.h>
#include <typeindex>

template <typename T>
std::string Demangle()
{
	std::string name = typeid(T).name();

	int status = -4;

	std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name.c_str(), NULL, NULL, &status), std::free};

	return (status == 0) ? res.get() : name;
}

inline std::string Demangle(const std::type_index& type)
{
	const char* name = type.name();

	int status = -4;

	std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};

	return (status == 0) ? res.get() : name;
}

template <typename T>
std::string DemangleWithoutNamespace()
{
	std::string demangled = Demangle<T>();

	size_t pos = demangled.find_last_of("::");
	if (pos != std::string::npos)
	{
		return demangled.substr(pos + 1);
	}

	return demangled;
}

inline void SanitizeEnvironment(sol::state& lua, sol::environment& env)
{
	env["collectgarbage"] = sol::nil;
	env["dofile"] = sol::nil;
	env["loadfile"] = sol::nil;
	env["module"] = sol::nil;
	env["load"] = sol::nil;
	// env["require"] = sol::nil;
	// env["package"] = sol::nil;
	env["getfenv"] = sol::nil;
	env["setfenv"] = sol::nil;
	env["newproxy"] = sol::nil;
	env["rawset"] = sol::nil;
	env["rawget"] = sol::nil;

	sol::table metatable = lua.create_table();
	metatable["__index"] = lua.globals();
	metatable["__newindex"] = [](sol::table self, sol::object key, sol::object value)
	{
		self.raw_set(key, value);
	};
	metatable[sol::metatable_key] = sol::nil;
	metatable["__metatable"] = "locked";

	env[sol::metatable_key] = metatable;
	env["_G"] = env;
	env["_ENV"] = env;
}

namespace Lua
{
	inline sol::environment CreateEnvironment(sol::state& lua, const bool globals)
	{
		if (globals)
		{
			sol::environment env = sol::environment(lua, sol::create, lua.globals());

			SanitizeEnvironment(lua, env);

			return env;
		}

		sol::environment env(lua, sol::create);

		SanitizeEnvironment(lua, env);

		return env;
	}

	inline bool LoadFile(sol::state& lua, const char* name)
	{
		sol::protected_function_result result = lua.safe_script_file(name);

		if (result.valid())
		{
			return true;
		}

		sol::error e = result;
		LogColor(LOG_YELLOW, "Failed to load lua file ", name, " with error: ", e.what());

		return false;
	}

	inline bool LoadFile(sol::state& lua, sol::environment& env, const char* name)
	{
		sol::protected_function_result result = lua.safe_script_file(name, env);

		if (result.valid())
		{
			return true;
		}

		sol::error e = result;
		LogColor(LOG_YELLOW, "Failed to load lua file ", name, " with error: ", e.what());

		return false;
	}

	template <typename T>
	std::optional<T> GetValue(sol::state& lua, const char* key)
	{
		sol::object object = lua[key];
		if (object.is<T>())
		{
			return object.as<T>();
		}

		LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle<T>());

		return std::nullopt;
	}

	template <typename T>
	std::optional<T> GetValue(sol::environment& env, const char* key)
	{
		sol::object object = env[key];
		if (object.is<T>())
		{
			return object.as<T>();
		}

		LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle<T>());

		return std::nullopt;
	}

	template <typename O, typename T>
	std::optional<T> GetValueObjectValue(sol::state& lua, const char* objectKey, const char* key)
	{
		sol::object object = lua[objectKey];
		if (object.is<O>())
		{
			sol::object member = object.as<sol::table>()[key];
			if (member.is<T>())
			{
				return member.as<T>();
			}

			LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle<T>());

			return std::nullopt;
		}

		LogColor(LOG_YELLOW, "Type mismatch for: ", objectKey, " expected ", Demangle<O>());

		return std::nullopt;
	}

	template <typename O, typename T>
	std::optional<T> GetValueObjectValue(sol::environment& env, const char* objectKey, const char* key)
	{
		sol::object object = env[objectKey];
		if (object.is<O>())
		{
			sol::object member = object.as<sol::table>()[key];
			if (member.is<T>())
			{
				return member.as<T>();
			}

			LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle<T>());

			return std::nullopt;
		}

		LogColor(LOG_YELLOW, "Type mismatch for: ", objectKey, " expected ", Demangle<O>());

		return std::nullopt;
	}

	template <typename T>
	bool TypeExists(sol::state& lua)
	{
		std::string typeName = DemangleWithoutNamespace<T>();

		sol::object obj = lua[typeName];
		return obj.valid();
	}

	template <typename T>
	bool TypeExists(sol::environment& env)
	{
		std::string typeName = DemangleWithoutNamespace<T>();

		sol::object obj = env[typeName];
		return obj.valid();
	}

	inline bool ObjectExists(sol::state& lua, const char* key)
	{
		sol::object obj = lua[key];
		return obj.valid();
	}

	inline bool ObjectExists(sol::environment& env, const char* key)
	{
		sol::object obj = env[key];
		return obj.valid();
	}

	inline bool FunctionExists(sol::state& lua, const char* key)
	{
		sol::object obj = lua[key];
		return obj.valid() && obj.is<sol::function>();
	}

	inline bool FunctionExists(sol::environment& env, const char* key)
	{
		sol::object obj = env[key];
		return obj.valid() && obj.is<sol::function>();
	}

	template <bool log = true, typename... Args>
	bool CallFunction(sol::state& lua, const char* key, Args&&... args)
	{
		sol::protected_function function = lua[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{
				return true;
			}

			sol::error e = result;

			LogColor(LOG_YELLOW, "Invalid call of function ", key, " with error: ", e.what());

			return false;
		}

		if (log)
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");
		}

		return false;
	}

	template <bool log = true, typename... Args>
	bool CallFunction(sol::environment& env, const char* key, Args&&... args)
	{
		sol::protected_function function = env[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{
				return true;
			}

			sol::error e = result;

			LogColor(LOG_YELLOW, "Invalid call of function ", key, " with error: ", e.what());

			return false;
		}

		if (log)
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");
		}

		return false;
	}

	template <typename T, typename... Args>
	std::optional<T> CallFunctionWithReturn(sol::state& lua, const char* key, Args&&... args)
	{
		sol::protected_function function = lua[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{
				sol::object object = result.get<sol::object>();
				if (object.is<T>())
				{
					return object.as<T>();
				}

				LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected: ", Demangle<T>());

				return std::nullopt;
			}

			sol::error e = result;
			LogColor(LOG_YELLOW, "Invalid call of function: ", key, " with error: ", e.what());

			return std::nullopt;
		}

		LogColor(LOG_YELLOW, "Function ", key, " does not exists");

		return std::nullopt;
	}

	template <typename T, typename... Args>
	std::optional<T> CallFunctionWithReturn(sol::environment& env, const char* key, Args&&... args)
	{
		sol::protected_function function = env[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{
				sol::object object = result.get<sol::object>();
				if (object.is<T>())
				{
					return object.as<T>();
				}

				LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected: ", Demangle<T>());

				return std::nullopt;
			}

			sol::error e = result;
			LogColor(LOG_YELLOW, "Invalid call of function: ", key, " with error: ", e.what());

			return std::nullopt;
		}

		LogColor(LOG_YELLOW, "Function ", key, " does not exists");

		return std::nullopt;
	}

	template <typename T>
	void RegisterFunction(sol::state& lua, const char* key, T function)
	{
		lua[key] = function;
	}

	template <typename T>
	void RegisterFunction(sol::environment& env, const char* key, T function)
	{
		env[key] = function;
	}

	template <typename T, typename I, typename... Args>
	void RegisterMethod(sol::state& lua, const char* key, I& instance, T (I::*method)(Args...))
	{
		lua.set_function(key, method, &instance);
	}

	template <typename T, typename I, typename... Args>
	void RegisterMethod(sol::environment& env, const char* key, I& instance, T (I::*method)(Args...))
	{
		env.set_function(key, method, &instance);
	}

	template <typename T, typename... Args>
	void RegisterType(sol::state& lua, const char* name, Args&&... args)
	{
		lua.new_usertype<T>(name, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	void RegisterType(sol::environment& env, const char* name, Args&&... args)
	{
		env.new_usertype<T>(name, std::forward<Args>(args)...);
	}

	template <typename T>
	void BindObject(sol::state& lua, const char* name, T object)
	{
		lua[name] = object;
	}

	template <typename T>
	void BindObject(sol::environment& env, const char* name, T object)
	{
		env[name] = object;
	}
}