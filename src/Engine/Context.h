#pragma once

#include "entt/entt.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"

struct Context
{
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	Renderer& renderer;
	ResourceManger& resourceManger;
	SceneManager& sceneManager;
	SystemManager& systemManager;
};