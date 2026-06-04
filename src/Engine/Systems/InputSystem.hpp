#pragma once

#include "Engine/SystemManager.hpp"
#include "MyMath/MyVectors.hpp"
#include "Types.hpp"
#include <optional>

enum class InputType : u8
{
	NONE,
	KEY_PRESS,
	KEY_PRESS_REPEAT,
	KEY_DOWN,
	KEY_RELEASE,
	KEY_UP,
	AXIS,
	POSITION,
	DELTA
};

enum class InputObject : u8
{
	KEYBOARD,
	MOUSE,
	GAMEPAD,
};

struct Input
{
	InputType type = InputType::NONE;
	InputObject object = InputObject::KEYBOARD;

	u32 key = 0;
	u32 negativeKey = 0;
	u32 gamepad = 0;

	constexpr bool operator<=>(const Input&) const = default;
};

struct InputState
{
	bool boolean = false;
	float number = 0;
	Vec2<float> vector;

	constexpr bool operator<=>(const InputState&) const = default;
};

class InputSystem : public System
{
public:

	void Update(const float deltaT) override;

	void BindInput(const std::string& name, const Input& input);
	void UnbindInput(const std::string& name);

	std::optional<bool> GetBoolInput(const std::string& name);
	std::optional<float> GetNumberInput(const std::string& name);
	std::optional<Vec2<float>> GetVectorInput(const std::string& name);

	void SetScaling(const float scale, const Vec2<float> offset);

private:

	static void UpdateKeyboard(Input& input, InputState& state);
	void UpdateMouse(Input& input, InputState& state);
	static void UpdateGamepad(Input& input, InputState& state);

	std::unordered_map<std::string, std::vector<Input>> m_inputs;
	std::unordered_map<std::string, InputState> m_states;

	Vec2<float> m_scaledMousePos;
	float m_scale = 1;
	Vec2<float> m_offset;
};