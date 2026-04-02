#include "Renderer.hpp"

#include "Utils/RaylibUtils.hpp"

#include "Components.hpp"
#include "raylib.h"

Renderer::Renderer(entt::registry& registry, const float virutalWidth, const float virutalHeight)
{
	camera.target = {0, 0};
	camera.offset = {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;

	m_virtualWidth = virutalWidth;
	m_virtualHeight = virutalHeight;

	registry.on_construct<Component::Sprite>().connect<&Renderer::MarkNeedSort>(this);
	registry.on_update<Component::Sprite>().connect<&Renderer::MarkNeedSort>(this);
	registry.on_destroy<Component::Sprite>().connect<&Renderer::MarkNeedSort>(this);
}

void Renderer::Update(entt::registry& registry)
{
	auto group = registry.group<Component::Sprite>();

	for (auto [entity, sprite] : group.each())
	{
		if (!IsTextureValid(sprite.texture))
		{
			Image image = GenImageColor(sprite.rectangle.width, sprite.rectangle.height, PURPLE);
			ImageDrawRectangleRec(&image,
			Rectangle{0, 0, static_cast<float>(sprite.rectangle.width * 0.5),
			static_cast<float>(sprite.rectangle.height * 0.5)},
			BLACK);
			ImageDrawRectangleRec(&image,
			Rectangle{static_cast<float>(sprite.rectangle.width * 0.5),
			static_cast<float>(sprite.rectangle.height * 0.5), static_cast<float>(sprite.rectangle.width * 0.5),
			static_cast<float>(sprite.rectangle.height * 0.5)},
			BLACK);

			sprite.texture = LoadTextureFromImage(image);
			UnloadImage(image);
		}
	}

	if (m_needSort)
	{
		SortSprites(registry);

		m_needSort = false;
	}
}

void Renderer::Draw(entt::registry& registry) const
{
	auto group = registry.group<Component::Sprite>(entt::get<Component::Transform>);

	Rectangle cameraRectangle = {.x = camera.target.x - camera.offset.x / camera.zoom,
	.y = camera.target.y - camera.offset.y / camera.zoom,
	.width = m_virtualWidth / camera.zoom,
	.height = m_virtualHeight / camera.zoom};

	BeginMode2D(camera);

	for (auto [entity, sprite, transform] : group.each())
	{
		if (IsRectangleVisible(sprite.rectangle, sprite.scale, transform.position.Raylib(), cameraRectangle))
		{
			DrawTextureRotScaleSelect(sprite.texture, sprite.rectangle, transform.position.Raylib(), transform.rotation,
			sprite.scale, sprite.color);
		}
	}

	EndMode2D();
}

void Renderer::SortSprites(entt::registry& registry)
{
	registry.sort<Component::Sprite>([](const Component::Sprite& a, const Component::Sprite& b)
	{
		if (a.layer != b.layer)
		{
			return a.layer < b.layer;
		}

		return a.texture.id < b.texture.id;
	});
}

void Renderer::MarkNeedSort([[maybe_unused]] entt::entity entity)
{
	m_needSort = true;
}