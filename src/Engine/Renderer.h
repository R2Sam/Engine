#pragma once

#include "entt/entt.h"
#include "raylib.h"

class Renderer
{
public:

	Renderer(entt::registry& registry);

	void Update(entt::registry& registry);

	void Draw(entt::registry& registry) const;

private:

	static void SortSprites(entt::registry& registry);

	void MarkNeedSort(entt::entity entity);

public:

	Camera2D camera;

private:

	bool _needSort = false;
};