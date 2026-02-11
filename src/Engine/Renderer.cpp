#include "Renderer.h"

#include "Utils/RaylibUtils.h"

#include "Components.h"
#include "raylib.h"

Renderer::Renderer(entt::registry& registry)
{
	camera.target = {0, 0};
	camera.offset = {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;

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
			ImageDrawRectangleRec(&image, Rectangle{0, 0, sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5}, BLACK);
			ImageDrawRectangleRec(&image, Rectangle{sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5, sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5}, BLACK);

			sprite.texture = LoadTextureFromImage(image);
			UnloadImage(image);
		}
	}

	if (_needSort)
	{
		SortSprites(registry);

		_needSort = false;
	}
}

void Renderer::Draw(entt::registry& registry) const
{
	auto group = registry.group<Component::Sprite>(entt::get<Component::Transform>);

	BeginMode2D(camera);

	for (auto [entity, sprite, transform] : group.each())
	{
		if (IsRectangleVisible(sprite.rectangle, sprite.scale, transform.position.vec2(), camera))
		{
			DrawTextureRotScaleSelect(sprite.texture, sprite.rectangle, transform.position.vec2(), transform.rotation, sprite.scale, sprite.color);
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

void Renderer::MarkNeedSort(entt::registry& registry, entt::entity entity)
{
	_needSort = true;
}