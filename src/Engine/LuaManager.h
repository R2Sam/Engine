#pragma once

// Forward
struct Context;
#include <cstdint> 
namespace entt
{
    enum class entity : uint32_t;
}

#include "Lua/MyLua.h"

#include <string>
#include <unordered_map>

struct LuaScript
{
	sol::environment environment;
	bool enabled = true;
};

class LuaManager
{
public:

	LuaManager();

	void Update(const float deltaT);

	bool LoadScript(const char* path);
	bool LoadEntityScript(entt::entity& entity, const char* path);

	void RemoveScript(const char* path);
	void RemoveEntityScript(entt::entity& entity);

	void ReloadScripts();
	void RelaodEntityScripts();

	void SetContext(Context& context);

public:

	sol::state lua;

private:

	void RegisterEngineAPIs();

private:

	Context* _context;

	std::unordered_map<std::string, LuaScript> _scripts;
};