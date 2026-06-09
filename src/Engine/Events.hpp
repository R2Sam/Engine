#pragma once

#include "Lua/MyLua.hpp"

/**
 * @file Events.hpp
 * @brief Engine events.
 */

namespace Event
{
	/**
	 * @brief Event signalling that the game should close
	 *
	 * Trigger this event to request a clean shutdown. The Engine listens for it
	 * and exits the main loop at the end of the current frame.
	 *
	 * Usage:
	 * @code
	 * DISPATCHER.trigger<Event::CloseGame>();
	 * @endcode
	 */
	struct CloseGame
	{
		/**
		 * @brief Registers the CloseGame type with Lua
		 *
		 * Called automatically by LuaManager::RegisterReceiveEvent and
		 * LuaManager::RegisterSendEvent. After registration, scripts can
		 * construct and trigger the event via "TriggerCloseGameEvent()".
		 *
		 * @param lua Lua state to register into
		 */
		static void LuaRegister(sol::state& lua)
		{
			Lua::RegisterType<Event::CloseGame>(lua, DemangleWithoutNamespace<Event::CloseGame>().c_str(),
			sol::constructors<CloseGame()>());
		}
	};
}