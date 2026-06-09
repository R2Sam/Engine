#pragma once

#include "Components.hpp"
#include "Registry.hpp"
#include "entt/entt.hpp"
#include "raylib.h"

/**
 * @file Renderer.hpp
 * @brief Sprite rendering.
 */

/**
 * @brief Draws all entities with Sprite and Transform components
 *
 * Manages a sorted sprite pool and a 2D camera. Sprites are sorted by layer
 * then by texture ID so draw calls are batched as much as possible.
 * Entities missing a valid texture are rendered with a purple/black checkerboard
 * placeholder until one is assigned.
 *
 * The camera is public so scenes can manipulate it directly.
 */
class Renderer
{
public:

	Camera2D camera;

	/**
	 * @brief Assigns or replaces the Sprite component on an entity
	 *
	 * Returns false and leaves the entity unchanged if the sprite's texture is invalid.
	 *
	 * @param entity Target entity
	 * @param sprite Sprite to assign
	 * @return True if the sprite was applied successfully
	 */
	static bool SetSprite(const Entity entity, const Component::Sprite& sprite);

	/**
	 * @brief Removes the Sprite component from an entity
	 *
	 * Does nothing if the entity does not have a sprite.
	 *
	 * @param entity Target entity
	 */
	static void RemoveSprite(const Entity entity);

	/**
	 * @brief Initialises the renderer and registers sprite change callbacks
	 *
	 * Called automatically by the Engine constructor. Registers OnConstruct,
	 * OnUpdate, and OnDestroy callbacks on Component::Sprite so the sprite pool
	 * is re-sorted whenever the set of sprites changes.
	 *
	 * @param registry Registry to watch for sprite changes
	 * @param virtualWidth Width of the virtual canvas in pixels
	 * @param virtualHeight Height of the virtual canvas in pixels
	 */
	void Init(Registry& registry, const float virtualWidth, const float virtualHeight);

private:

	Renderer(Registry& registry, const float virtualWidth, const float virtualHeight);

	/**
	 * @brief Re-sorts the sprite pool if any sprite was added, changed, or removed
	 *
	 * Called once per frame by the Engine before drawing begins.
	 *
	 * @param registry Registry holding the sprite pool
	 */
	void Update(Registry& registry);

	/**
	 * @brief Draws all visible sprites to the current render target
	 *
	 * Iterates entities with Sprite and Transform components in sorted order.
	 * Sprites outside the camera frustum are culled. Must be called between
	 * BeginTextureMode / EndTextureMode.
	 *
	 * @param registry Registry to query for Sprite and Transform components
	 */
	void Draw(Registry& registry) const;

	/**
	 * @brief Marks the sprite pool as needing a re-sort on the next Update
	 */
	void MarkNeedSort();

	bool m_needSort = false;

	float m_virtualWidth = 0;
	float m_virtualHeight = 0;

	friend class Engine;
};