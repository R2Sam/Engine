#pragma once

#include "Raylib/raylib.h"
#include "MyMath/MyVectors.h"

#include "Lua/MyLua.h"
#include "Types.h"

#include <string>

namespace Component
{
	struct Transform
	{
		Vector2f position;
		Vector2f velocity;
		float rotation = 0;
	};

	struct Sprite
	{
		Texture2D texture;
		Rectangle rectangle = {0, 0, 0, 0};
		Color color = WHITE;
		float scale = 1;
		u32 layer = 1;
	};

	struct LuaScript
	{
		sol::environment environment;
		std::string path;
		bool enabled = true;
	};
}