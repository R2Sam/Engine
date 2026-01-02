#include "Renderer.h"

#include "MyRaylib/MyRaylib.h"

#include "Raylib/raylib.h"
#include <cstdio>

Renderer::Renderer()
{
	camera.target = {0, 0};
	camera.offset =  {0, 0};
	camera.zoom = 1;
	camera.rotation = 0;
}

void Renderer::Draw(std::vector<std::pair<Component::Sprite, Component::Transform>>& sprites)
{
	BeginMode2D(camera);

	for (auto& [sprite, transform] : sprites)
	{
		if (!IsTextureValid(sprite.texture))
		{
			Image image = GenImageColor(sprite.rectangle.width, sprite.rectangle.height, PURPLE);
			ImageDrawRectangleRec(&image, Rectangle{0, 0, sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5}, BLACK);
			ImageDrawRectangleRec(&image, Rectangle{sprite.rectangle.width * 0.5, sprite.rectangle.height * 0.5, sprite.rectangle.width, sprite.rectangle.height}, BLACK);

			sprite.texture = LoadTextureFromImage(image);
			UnloadImage(image);
		}

		if (IsRectangleVisible(sprite.rectangle, sprite.scale, transform.position.vec2(), camera))
		{
			DrawTextureRotScaleSelect(sprite.texture, sprite.rectangle, transform.position.vec2(), transform.rotation, sprite.scale, sprite.color);
		}
	}

	EndMode2D();
}