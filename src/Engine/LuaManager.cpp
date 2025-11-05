#include "LuaManager.h"

#include "Context.h"
#include "Components.h"

#include "Assert.h"
#include "Lua/MyLua.h"
#include "Lua/sol/sol.hpp"

LuaManager::LuaManager()
{
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

	RegisterEngineAPIs();
}

void LuaManager::Update(const float deltaT)
{
	for (auto& [path, script] : _scripts)
	{
		if (script.enabled)
		{
			Lua::CallFunction(script.environment, "Update", deltaT);
		}
	}

	Assert(_context, "Context must be set first");

	auto view = _context->registry.view<Component::LuaScript>();
	for (auto [entity, script] : view.each())
	{
		if (!script.enabled)
		{
			continue;
		}

		Lua::CallFunction(script.environment, "Update", deltaT);
	}
}

bool LuaManager::LoadScript(const char* path) 
{
	LuaScript script;
	script.environment = Lua::CreateEnvironment(lua, true);

	if (!Lua::LoadFile(lua, script.environment, path))
	{
		return false;
	}

	_scripts.emplace(path, script);

	return true;
}

bool LuaManager::LoadEntityScript(entt::entity& entity, const char* path)
{
	Component::LuaScript& script = _context->registry.emplace_or_replace<Component::LuaScript>(entity);
	script.path = path;
	script.environment = Lua::CreateEnvironment(lua, true);

	Lua::BindObject(script.environment, "Entity", entity);

	if (!Lua::LoadFile(lua, script.environment, path))
    {
        _context->registry.remove<Component::LuaScript>(entity);

        return false;
    }

    return true;
}

void LuaManager::RemoveScript(const char* path) 
{
	_scripts.erase(path);
}

void LuaManager::RemoveEntityScript(entt::entity& entity) 
{
	_context->registry.remove<Component::LuaScript>(entity);
}

void LuaManager::ReloadScripts() 
{
	for (auto& [path, script] : _scripts)
	{
		Lua::LoadFile(lua, script.environment, path);
	}
}

void LuaManager::RelaodEntityScripts() 
{
	auto view = _context->registry.view<Component::LuaScript>();
	for (auto [entity, script] : view.each())
	{
		Lua::LoadFile(lua, script.environment, script.path);
	}
}

void LuaManager::SetContext(Context& context)
{
	_context = &context;
}

void LuaManager::RegisterEngineAPIs()
{
	// Basic types
	Lua::RegisterType<Vector2f>(lua, "Vector2f",
		sol::constructors<Vector2f(), Vector2f(float, float)>(),
		"x", &Vector2f::x,
		"y", &Vector2f::y);


	// Components
	Lua::RegisterType<Component::Transform>(lua, "Transform",
		"position", &Component::Transform::position,
		"velocity", &Component::Transform::velocity,
		"rotation", &Component::Transform::rotation
	);

	// Ecs
	Lua::RegisterType<entt::entity>(lua, "Entity",
		"id", entt::to_integral<entt::entity>
	);

	Lua::RegisterFunction(lua, "GetTransform", [this](entt::entity entity) -> Component::Transform& {
		return _context->registry.get<Component::Transform>(entity);
	});

	Lua::RegisterFunction(lua, "HasTransform", [this](entt::entity entity) -> bool {
		return _context->registry.all_of<Component::Transform>(entity);
	});
}