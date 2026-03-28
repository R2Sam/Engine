#pragma once

#include "entt/entt.h"
#include "raylib.h"

class Renderer
{
public:

	Camera2D camera;

private:

	Renderer(entt::registry& registry);

	void Update(entt::registry& registry);

	void Draw(entt::registry& registry) const;

	static void SortSprites(entt::registry& registry);

	void MarkNeedSort(entt::entity entity);

	bool m_needSort = false;

	friend class Engine;
};