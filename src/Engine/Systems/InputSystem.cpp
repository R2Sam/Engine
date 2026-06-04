#include "InputSystem.hpp"
#include "raylib.h"
#include <cstdlib>

void InputSystem::Update([[maybe_unused]] const float deltaT)
{
	Vec2<float> mousePos = GetMousePosition();
	m_scaledMousePos = (mousePos - m_offset) / m_scale;

	m_states.clear();

	for (auto& [name, inputs] : m_inputs)
	{
		InputState state;

		for (Input& input : inputs)
		{
			switch (input.object)
			{
			case InputObject::KEYBOARD:
			{
				UpdateKeyboard(input, state);
			}
			break;

			case InputObject::MOUSE:
			{
				UpdateMouse(input, state);
			}
			break;

			case InputObject::GAMEPAD:
			{
				UpdateGamepad(input, state);
			}
			break;
			}
		}

		m_states[name] = state;
	}
}

void InputSystem::BindInput(const std::string& name, const Input& input)
{
	if (!m_inputs.contains(name))
	{
		m_inputs.emplace(name, std::vector<Input>{input});
		return;
	}

	m_inputs[name].emplace_back(input);
}

void InputSystem::UnbindInput(const std::string& name)
{
	m_inputs.erase(name);
	m_states.erase(name);
}

std::optional<bool> InputSystem::GetBoolInput(const std::string& name)
{
	if (!m_states.contains(name))
	{
		return std::nullopt;
	}

	return m_states[name].boolean;
}

std::optional<float> InputSystem::GetNumberInput(const std::string& name)
{
	if (!m_states.contains(name))
	{
		return std::nullopt;
	}

	return m_states[name].number;
}

std::optional<Vec2<float>> InputSystem::GetVectorInput(const std::string& name)
{
	if (!m_states.contains(name))
	{
		return std::nullopt;
	}

	return m_states[name].vector;
}

void InputSystem::UpdateKeyboard(Input& input, InputState& state)
{
	switch (input.type)
	{
	case InputType::NONE:
	{
	}
	break;

	case InputType::KEY_PRESS:
	{
		state.boolean = state.boolean || IsKeyPressed(input.key);
	}
	break;

	case InputType::KEY_PRESS_REPEAT:
	{
		state.boolean = state.boolean || IsKeyPressedRepeat(input.key);
	}
	break;

	case InputType::KEY_DOWN:
	{
		state.boolean = state.boolean || IsKeyDown(input.key);
	}
	break;

	case InputType::KEY_RELEASE:
	{
		state.boolean = state.boolean || IsKeyReleased(input.key);
	}
	break;

	case InputType::KEY_UP:
	{
		state.boolean = state.boolean || IsKeyUp(input.key);
	}
	break;

	case InputType::AXIS:
	{
		float thisAxis = 0;
		thisAxis += IsKeyDown(input.key);
		thisAxis -= IsKeyDown(input.negativeKey);

		if (std::abs(thisAxis) > std::abs(state.number))
		{
			state.number = thisAxis;
		}
	}
	break;

	case InputType::POSITION:
	{
	}
	break;

	case InputType::DELTA:
	{
	}
	break;
	}
}

void InputSystem::UpdateMouse(Input& input, InputState& state)
{
	switch (input.type)
	{
	case InputType::NONE:
	{
	}
	break;

	case InputType::KEY_PRESS:
	{
		state.boolean = state.boolean || IsMouseButtonPressed(input.key);
	}
	break;

	case InputType::KEY_PRESS_REPEAT:
	{
	}
	break;

	case InputType::KEY_DOWN:
	{
		state.boolean = state.boolean || IsMouseButtonDown(input.key);
	}
	break;

	case InputType::KEY_RELEASE:
	{
		state.boolean = state.boolean || IsMouseButtonReleased(input.key);
	}
	break;

	case InputType::KEY_UP:
	{
		state.boolean = state.boolean || IsMouseButtonUp(input.key);
	}
	break;

	case InputType::AXIS:
	{
		float thisAxis = GetMouseWheelMove();

		if (std::abs(thisAxis) > std::abs(state.number))
		{
			state.number = thisAxis;
		}
	}
	break;

	case InputType::POSITION:
	{
		state.vector = m_scaledMousePos;
	}
	break;

	case InputType::DELTA:
	{
		state.vector = GetMouseDelta();
		state.vector *= m_scale;
	}
	break;
	}
}

void InputSystem::UpdateGamepad(Input& input, InputState& state)
{
	if (!IsGamepadAvailable(input.gamepad))
	{
		return;
	}

	switch (input.type)
	{
	case InputType::NONE:
	{
	}
	break;

	case InputType::KEY_PRESS:
	{
		state.boolean = state.boolean || IsGamepadButtonPressed(input.gamepad, input.key);
	}
	break;

	case InputType::KEY_PRESS_REPEAT:
	{
	}
	break;

	case InputType::KEY_DOWN:
	{
		state.boolean = state.boolean || IsGamepadButtonDown(input.gamepad, input.key);
	}
	break;

	case InputType::KEY_RELEASE:
	{
		state.boolean = state.boolean || IsGamepadButtonReleased(input.gamepad, input.key);
	}
	break;

	case InputType::KEY_UP:
	{
		state.boolean = state.boolean || IsGamepadButtonUp(input.gamepad, input.key);
	}
	break;

	case InputType::AXIS:
	{
		float thisAxis = GetGamepadAxisMovement(input.gamepad, input.key);

		if (std::abs(thisAxis) > std::abs(state.number))
		{
			state.number = thisAxis;
		}
	}
	break;

	case InputType::POSITION:
	{
	}
	break;

	case InputType::DELTA:
	{
	}
	break;
	}
}

void InputSystem::SetScaling(const float scale, const Vec2<float> offset)
{
	m_scale = scale;
	m_offset = offset;
}