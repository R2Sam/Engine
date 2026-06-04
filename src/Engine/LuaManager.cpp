#include "LuaManager.hpp"

#include "sol/sol.hpp"
#include <optional>

LuaManager::LuaManager()
{
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package);
}

void LuaManager::Update(const float deltaT)
{
	{
		std::queue<std::function<void()>> toFlush;
		{
			std::unique_lock lock(m_mutex);
			std::swap(toFlush, m_pendingEvents);
		}

		while (!toFlush.empty())
		{
			toFlush.front()();
			toFlush.pop();
		}
	}

	std::unique_lock lock(m_mutex);

	for (auto& [path, script] : m_scripts)
	{
		if (script.enabled)
		{
			Lua::CallFunction<false>(script.environment, "Update", deltaT);
		}
	}
}

bool LuaManager::LoadScript(const std::string& path)
{
	{
		std::unique_lock lock(m_mutex);

		// Don't load the same script twice
		if (m_scripts.contains(path))
		{
			return false;
		}
	}

	LuaScript script;

	std::string directory = path.substr(0, path.find_last_of("/\\") + 1);
	script.directory = directory;
	script.environment = Lua::CreateEnvironment(lua, true);

	script.environment["require"] = [this, directory](const std::string& modPath) -> sol::object
	{
		if (modPath.contains(".."))
		{
			LogColor(LOG_YELLOW, "Sandboxed require blocked path: ", modPath);
			return sol::nil;
		}

		std::string fullPath = directory + modPath + ".lua";

		sol::environment moduleEnv = Lua::CreateEnvironment(lua, true);
		if (!Lua::LoadFile(lua, moduleEnv, fullPath.c_str()))
		{
			return sol::nil;
		}

		return moduleEnv;
	};

	if (!Lua::LoadFile(lua, script.environment, path.c_str()))
	{
		return false;
	}

	std::unique_lock lock(m_mutex);

	m_scripts.emplace(path, std::move(script));

	return true;
}

void LuaManager::RemoveScript(const std::string& path)
{
	std::unique_lock lock(m_mutex);

	m_scripts.erase(path);
}

bool LuaManager::IsScriptLoaded(const std::string& path)
{
	std::unique_lock lock(m_mutex);

	auto it = m_scripts.find(path);
	return it != m_scripts.end();
}

void LuaManager::EnableScript(const std::string& path)
{
	std::unique_lock lock(m_mutex);

	auto it = m_scripts.find(path);
	if (it == m_scripts.end())
	{
		return;
	}

	it->second.enabled = true;
}

void LuaManager::DisableScript(const std::string& path)
{
	std::unique_lock lock(m_mutex);

	auto it = m_scripts.find(path);
	if (it == m_scripts.end())
	{
		return;
	}

	it->second.enabled = false;
}

std::optional<sol::environment> LuaManager::GetScriptEnvironment(const std::string& path)
{
	std::unique_lock lock(m_mutex);

	auto it = m_scripts.find(path);
	if (it == m_scripts.end())
	{
		return std::nullopt;
	}

	return it->second.environment;
}

void LuaManager::ReloadScripts()
{
	std::unique_lock lock(m_mutex);

	for (auto& [path, script] : m_scripts)
	{
		script.environment = Lua::CreateEnvironment(lua, true);

		std::string directory = script.directory;
		script.environment["require"] = [this, directory](const std::string& modPath) -> sol::object
		{
			if (modPath.contains(".."))
			{
				LogColor(LOG_YELLOW, "Sandboxed require blocked path: ", modPath);
				return sol::nil;
			}

			std::string fullPath = directory + modPath + ".lua";

			sol::environment moduleEnv = Lua::CreateEnvironment(lua, true);
			if (!Lua::LoadFile(lua, moduleEnv, fullPath.c_str()))
			{
				return sol::nil;
			}

			return moduleEnv;
		};

		Lua::LoadFile(lua, script.environment, path.c_str());
	}
}