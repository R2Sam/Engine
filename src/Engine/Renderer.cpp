#include "Renderer.h"

#include "MyRaylib/MyRaylib.h"

#include "Components.h"

Renderer::Renderer()
{
	camera.target = {0, 0};
	camera.offset =  {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;
}

void Renderer::Draw(entt::registry& registry)
{
	auto view = registry.view<const Component::Transform, const Component::Sprite>();

	for (auto [entity, transform, sprite] : view.each())
	{
		if (IsRectangleVisible(sprite.rectangle, sprite.scale, transform.position.vec2(), camera))
		{
			DrawTextureRotScaleSelect(sprite.texture, sprite.rectangle, transform.position.vec2(), transform.rotation, sprite.scale, sprite.color);
		}
	}
}