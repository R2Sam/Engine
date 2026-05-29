#pragma once

#include "Lua/MyLua.hpp"

namespace Event
{
	/**
	 * @brief Event signalling that the game should close
	 */

	struct CloseGame
	{
		/**
		 * @brief Registers the event type with Lua
		 *
		 * This is called by lua when registering an event with the lua manager.
		 *
		 * @param lua Lua state to register into
		 */

		static void LuaRegister(sol::state& lua)
		{
			Lua::RegisterType<Event::CloseGame>(lua, DemangleWithoutNamespace<Event::CloseGame>().c_str(),
			sol::constructors<CloseGame()>());
		}
	};

	struct InputAction
	{
		std::string name;
		float value = 0;
	};
}