#pragma once

#define SOL_NO_EXCEPTIONS 1
#include "sol/sol.hpp"

#include "Log/Log.h"

#include <cxxabi.h>

inline std::string Demangle(const char* name)
{

    int status = -4;

    std::unique_ptr<char, void(*)(void*)> res{
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}

inline void SanitizeEnvironment(sol::environment& env)
{
	env["os"] = sol::nil;
	env["io"] = sol::nil;
	env["debug"] = sol::nil;
	env["package"] = sol::nil;
	env["dofile"] = sol::nil;
	env["load"] = sol::nil;
	env["loadfile"] = sol::nil;
	env["loadstring"] = sol::nil;
	env["rawget"] = sol::nil;
	env["collectgarbage"] = sol::nil;
}

namespace Lua
{
	inline sol::environment CreateEnvironment(sol::state& lua, const bool globals)
	{
		if (globals)
		{
			sol::environment env = sol::environment(lua, sol::create, lua.globals());

			SanitizeEnvironment(env);

	    	return env;
		}

		sol::environment env(lua, sol::create);

		SanitizeEnvironment(env);

	    return env;
	}

	inline bool LoadFile(sol::state& lua, const std::string& name)
	{
		sol::protected_function_result result = lua.safe_script_file(name);

		if (result.valid())
		{
			return true;
		}

		else
		{
			sol::error e = result;
			LogColor(LOG_YELLOW, "Failed to load lua file ", name, " with error: ", e.what());

			return false;
		}
	}

	inline bool LoadFile(sol::state& lua, sol::environment& env, const std::string& name)
	{
		sol::protected_function_result result = lua.safe_script_file(name, env);

		if (result.valid())
		{
			return true;
		}

		else
		{
			sol::error e = result;
			LogColor(LOG_YELLOW, "Failed to load lua file ", name, " with error: ", e.what());

			return false;
		}
	}

	template<typename T>
	std::optional<T> GetValue(sol::state& lua, const std::string& key)
	{
		sol::object object = lua[key];
		if (object.is<T>())
		{
			return object.as<T>();
		}

		else
		{
			LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle(typeid(T).name()));

			return std::nullopt;
		}
	}

	template<typename T>
	std::optional<T> GetValue(sol::environment& env, const std::string& key)
	{
		sol::object object = env[key];
		if (object.is<T>())
		{
			return object.as<T>();
		}

		else
		{
			LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle(typeid(T).name()));

			return std::nullopt;
		}
	}

	template<typename O, typename T>
	std::optional<T> GetValueObjectValue(sol::state& lua, const std::string& objectKey, const std::string& key)
	{
		sol::object object = lua[objectKey];
		if (object.is<O>())
		{
			sol::object member = object.as<sol::table>()[key];
			if (member.is<T>())
			{
				return member.as<T>();
			}

			else
			{
				LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle(typeid(T).name()));

				return std::nullopt;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Type mismatch for: ", objectKey, " expected ", Demangle(typeid(O).name()));

			return std::nullopt;
		}
	}

	template<typename O, typename T>
	std::optional<T> GetValueObjectValue(sol::environment& env, const std::string& objectKey, const std::string& key)
	{
		sol::object object = env[objectKey];
		if (object.is<O>())
		{
			sol::object member = object.as<sol::table>()[key];
			if (member.is<T>())
			{
				return member.as<T>();
			}

			else
			{
				LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected ", Demangle(typeid(T).name()));

				return std::nullopt;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Type mismatch for: ", objectKey, " expected ", Demangle(typeid(O).name()));

			return std::nullopt;
		}
	}

	template<typename... Args>
	bool CallFunction(sol::state& lua, const std::string& key, Args&&... args)
	{
		sol::protected_function function = lua[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{	
				return true;
			}

			else
			{
				sol::error e = result;
				LogColor(LOG_YELLOW, "Invalid call of function ", key, " with error: ", e.what());

				return false;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");

			return false;
		}
	}

	template<typename... Args>
	bool CallFunction(sol::environment& env, const std::string& key, Args&&... args)
	{
		sol::protected_function function = env[key];
		if (function)
		{
			sol::protected_function_result result = function(std::forward<Args>(args)...);
			if (result.valid())
			{	
				return true;
			}

			else
			{
				sol::error e = result;
				LogColor(LOG_YELLOW, "Invalid call of function ", key, " with error: ", e.what());

				return false;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");

			return false;
		}
	}

	template<typename T, typename... Args>
	std::optional<T> CallFunctionWithReturn(sol::state& lua, const std::string& key, Args&&... args)
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

				else
				{
					LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected: ", Demangle(typeid(T).name()));

					return std::nullopt;
				}
			}

			else
			{
				sol::error e = result;
				LogColor(LOG_YELLOW, "Invalid call of function: ", key, " with error: ", e.what());

				return std::nullopt;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");

			return std::nullopt;
		}
	}

	template<typename T, typename... Args>
	std::optional<T> CallFunctionWithReturn(sol::environment& env, const std::string& key, Args&&... args)
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

				else
				{
					LogColor(LOG_YELLOW, "Type mismatch for: ", key, " expected: ", Demangle(typeid(T).name()));

					return std::nullopt;
				}
			}

			else
			{
				sol::error e = result;
				LogColor(LOG_YELLOW, "Invalid call of function: ", key, " with error: ", e.what());

				return std::nullopt;
			}
		}

		else
		{
			LogColor(LOG_YELLOW, "Function ", key, " does not exists");

			return std::nullopt;
		}
	}

	template<typename T>
	void RegisterFunction(sol::state& lua, const std::string& key, T function)
	{
		lua[key] = function;
	}

	template<typename T>
	void RegisterFunction(sol::environment& env, const std::string& key, T function)
	{
		env[key] = function;
	}

	template<typename T, typename I, typename... Args>
	void RegisterMethod(sol::state& lua, const std::string& key, I& instance, T (I::*method)(Args...))
	{
	    lua.set_function(key, method, &instance);
	}


	template<typename T, typename I, typename... Args>
	void RegisterMethod(sol::environment& env, const std::string& key, I& instance, T (I::*method)(Args...))
	{
	    env.set_function(key, method, &instance);
	}

	template<typename T, typename... Args>
	void RegisterType(sol::state& lua, const std::string& name, Args&&... args)
	{
		lua.new_usertype<T>(name, std::forward<Args>(args)...);
	}


	template<typename T, typename... Args>
	void RegisterType(sol::environment& env, const std::string& name, Args&&... args)
	{
		env.new_usertype<T>(name, std::forward<Args>(args)...);
	}

	template<typename T>
	void BindObject(sol::state& lua, const std::string& name, T object)
	{
		lua[name] = object;
	}

	template<typename T>
	void BindObject(sol::environment& env, const std::string& name, T object)
	{
		env[name] = object;
	}
}