#pragma once

#include "entt/entt.h"
#include "raylib.h"

class Renderer
{
public:

	Camera2D camera;

private:

	Renderer(entt::registry& registry, const float virutalWidth, const float virutalHeight);

	void Update(entt::registry& registry);

	void Draw(entt::registry& registry) const;

	static void SortSprites(entt::registry& registry);

	void MarkNeedSort(entt::entity entity);

	bool m_needSort = false;

	float m_virtualWidth = 0;
	float m_virtualHeight = 0;

	friend class Engine;
};