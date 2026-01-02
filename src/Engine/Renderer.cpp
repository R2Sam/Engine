#include "Renderer.h"

#include "MyRaylib/MyRaylib.h"

#include "Components.h"
#include "Raylib/raylib.h"

Renderer::Renderer()
{
	camera.target = {0, 0};
	camera.offset =  {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;
}

void Renderer::Update(entt::registry& registry) 
{
	auto view = registry.view<Component::Sprite>();

	for (const entt::entity entity : view)
	{
		Component::Sprite& sprite = registry.get<Component::Sprite>(entity);

		if (!IsTextureValid(sprite.texture))
		{
			Image image = GenImageColor(sprite.rectangle.width, sprite.rectangle.height, PURPLE);
			ImageDrawRectangleRec(&image, Rectangle{0, 0, sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5}, BLACK);
			ImageDrawRectangleRec(&image, Rectangle{sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5, sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5}, BLACK);

			sprite.texture = LoadTextureFromImage(image);
			UnloadImage(image);
		}
	}
}

void Renderer::Draw(entt::registry& registry)
{
	SortSprites(registry);

	auto group = registry.group<Component::Sprite>(entt::get<const Component::Transform>);

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
        return a.layer < b.layer;
    });
}