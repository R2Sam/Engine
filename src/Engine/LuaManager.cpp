#include "LuaManager.h"

#include "Lua/MyLua.h"
#include "Lua/sol/sol.hpp"
#include <optional>

LuaManager::LuaManager()
{
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package);
}

void LuaManager::Update(const float deltaT)
{
	for (auto& [path, script] : _scripts)
	{
		if (script.enabled)
		{
			Lua::CallFunction<false>(script.environment, "Update", deltaT);
		}
	}
}

bool LuaManager::LoadScript(const char* path) 
{
	auto it = _scripts.find(path);
	if (it != _scripts.end())
	{
		return false;
	}

	LuaScript script;
	script.environment = Lua::CreateEnvironment(lua, true);

	if (!Lua::LoadFile(lua, script.environment, path))
	{
		return false;
	}

	_scripts.emplace(path, script);

	return true;
}

void LuaManager::RemoveScript(const char* path) 
{
	_scripts.erase(path);
}

bool LuaManager::IsScriptLoaded(const char* path) 
{
	auto it = _scripts.find(path);
	if (it == _scripts.end())
	{
		return false;
	}

	return true;
}

void LuaManager::EnableScript(const char* path) 
{
	auto it = _scripts.find(path);
	if (it == _scripts.end())
	{
		return;
	}

	it->second.enabled = true;
}

void LuaManager::DisableScript(const char* path) 
{
	auto it = _scripts.find(path);
	if (it == _scripts.end())
	{
		return;
	}

	it->second.enabled = false;
}

std::optional<sol::environment> LuaManager::GetScriptEnvironment(const char* path) 
{
	auto it = _scripts.find(path);
	if (it == _scripts.end())
	{
		return std::nullopt;
	}

	return it->second.environment;
}

void LuaManager::ReloadScripts() 
{
	for (auto& [path, script] : _scripts)
	{
		Lua::LoadFile(lua, script.environment, path.c_str());
	}
}