#pragma once

#include "Engine/SystemManager.hpp"
#include "MyMath/MyVectors.hpp"
#include "Types.hpp"
#include <optional>

/**
 * @file InputSystem.hpp
 * @brief Abstract input binding system supporting keyboard, mouse and gamepad.
 */

/**
 * @enum InputType
 * @brief Types of input queries.
 */
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

/**
 * @enum InputObject
 * @brief Input device categories.
 */
enum class InputObject : u8
{
	KEYBOARD,
	MOUSE,
	GAMEPAD,
};

/**
 * @struct Input
 * @brief Describes a single input binding (device, key/axis, type).
 */
struct Input
{
	InputType type = InputType::NONE;
	InputObject object = InputObject::KEYBOARD;

	u32 key = 0;
	u32 negativeKey = 0;
	u32 gamepad = 0;

	constexpr bool operator<=>(const Input&) const = default;
};

/**
 * @struct InputState
 * @brief Aggregated state for a named binding.
 */
struct InputState
{
	bool boolean = false;
	float number = 0;
	Vec2<float> vector;

	constexpr bool operator<=>(const InputState&) const = default;
};

/**
 * @class InputSystem
 * @brief Aggregates multiple physical inputs into logical actions.
 *
 * Allows binding several keys/buttons/axes to one logical name.
 * Booleans are OR‑combined, axes keep the highest absolute value,
 * vector inputs overwrite.
 */
class InputSystem : public System
{
public:

	/**
	 * @brief Updates all input states.
	 * @param deltaT Time since last update (unused).
	 *
	 * Reads current mouse position (scaled), then evaluates every bound input
	 * and aggregates the results into per‑name InputState structures.
	 */
	void Update(const float deltaT) override;

	/**
	 * @brief Adds an input to a logical binding.
	 * @param name Logical action name.
	 * @param input Input description (device, key, type).
	 *
	 * Multiple inputs can be bound to the same name.
	 */
	void BindInput(const std::string& name, const Input& input);

	/**
	 * @brief Removes a logical binding entirely.
	 * @param name Action name.
	 */
	void UnbindInput(const std::string& name);

	/**
	 * @brief Returns the boolean state of a logical binding.
	 * @param name Action name.
	 * @return std::nullopt if binding does not exist, otherwise the current boolean state.
	 */
	std::optional<bool> GetBoolInput(const std::string& name);

	/**
	 * @brief Returns the float axis state of a logical binding.
	 * @param name Action name.
	 * @return std::nullopt if binding does not exist, otherwise the current axis value.
	 */
	std::optional<float> GetNumberInput(const std::string& name);

	/**
	 * @brief Returns the vector (position/delta) state of a logical binding.
	 * @param name Action name.
	 * @return std::nullopt if binding does not exist, otherwise the current vector.
	 */
	std::optional<Vec2<float>> GetVectorInput(const std::string& name);

	/**
	 * @brief Sets mouse scaling and offset for POSITION/DELTA queries.
	 * @param scale Scale factor applied to mouse coordinates.
	 * @param offset Pixel offset added before scaling.
	 */
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