#pragma once

#include "Components.hpp"
#include "Registry.hpp"
#include "entt/entt.hpp"
#include "raylib.h"

using Entity = entt::entity;

class Renderer
{
public:

	Camera2D camera;

	static bool SetSprite(const Entity entity, const Component::Sprite& sprite);
	static void RemoveSprite(const Entity entity);

private:

	Renderer(Registry& registry, const float virutalWidth, const float virutalHeight);

	void Update(Registry& registry);

	void Draw(Registry& registry) const;

	static void SortSprites(Registry& registry);

	void MarkNeedSort();

	bool m_needSort = false;

	float m_virtualWidth = 0;
	float m_virtualHeight = 0;

	friend class Engine;
};