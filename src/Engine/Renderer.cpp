#include "Renderer.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Registry.hpp"
#include "Utils/RaylibUtils.hpp"

#include "Components.hpp"
#include "raylib.h"

bool Renderer::SetSprite(const Entity entity, const Component::Sprite& sprite)
{
	if (!IsTextureValid(sprite.texture))
	{
		return false;
	}

	const Component::Sprite* oldSprite = REGISTRY.Get<Component::Sprite>(entity);
	if (oldSprite)
	{
		REGISTRY.Replace<Component::Sprite>(entity, sprite);
	}

	else
	{
		REGISTRY.Emplace<Component::Sprite>(entity, sprite);
	}

	return true;
}

void Renderer::RemoveSprite(const Entity entity)
{
	REGISTRY.Remove<Component::Sprite>(entity);
}

Renderer::Renderer(Registry& registry, const float virutalWidth, const float virutalHeight)
{
	camera.target = {0, 0};
	camera.offset = {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;

	m_virtualWidth = virutalWidth;
	m_virtualHeight = virutalHeight;

	registry.OnConstruct<Component::Sprite>([this](Component::Sprite&, const Entity)
	{
		MarkNeedSort();
	});

	registry.OnUpdate<Component::Sprite>([this](Component::Sprite&, const Entity)
	{
		MarkNeedSort();
	});

	registry.OnDestroy<Component::Sprite>([this](Component::Sprite&, const Entity)
	{
		MarkNeedSort();
	});

	REGISTRY.ForbidOwningComponent<Component::Sprite>();
}

void Renderer::Update(Registry& registry)
{
	auto group = registry.GetGroup(entt::get<Component::Sprite>);

	for (auto [entity, sprt] : group.each())
	{
		Component::Sprite sprite = sprt;

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

			REGISTRY.Replace<Component::Sprite>(entity, sprite);
		}
	}

	if (m_needSort)
	{
		SortSprites(registry);

		m_needSort = false;
	}
}

void Renderer::Draw(Registry& registry) const
{
	auto group = registry.GetGroup<Component::Sprite>(entt::get<Component::Transform>);

	Rectangle cameraRectangle = {.x = camera.target.x - (camera.offset.x / camera.zoom),
	.y = camera.target.y - (camera.offset.y / camera.zoom),
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

void Renderer::SortSprites(Registry& registry)
{
	registry.Sort<Component::Sprite>([](const Component::Sprite& a, const Component::Sprite& b)
	{
		if (a.layer != b.layer)
		{
			return a.layer < b.layer;
		}

		return a.texture.id < b.texture.id;
	});
}

void Renderer::MarkNeedSort()
{
	m_needSort = true;
}