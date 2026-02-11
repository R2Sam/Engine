#pragma once

#include "Types.h"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @brief Base class for all systems
 *
 * All systems must derive from this class and implement Update() and Draw().
 */

class System
{
public:

	virtual ~System()
	{
	}

	/**
	 * @brief Updates the system
	 *
	 * Called once per frame before rendering according to system priority.
	 * Called before scenes.
	 *
	 * @param deltaT Duration of previous frame
	 */

	virtual void Update(const float deltaT) = 0;

	/**
	 * @brief Renders the system
	 *
	 * Called once per frame after updating according to system priority
	 * Called before scenes
	 */

	virtual void Draw() = 0;
};

/**
 * @brief Manages execution and order of systems
 *
 * Systems are updated and rendered in order.
 * Lower priority values go first.
 * Systems update and render before scenes.
 */

class SystemManager
{
public:

	/**
	 * @brief Updates all systems in order
	 *
	 * @param deltaT Duration of previous frame
	 */

	void Update(const float deltaT);

	/**
	 * @brief Renders all systems in order
	 */
	void Draw();

	/**
	 * @brief Adds a system to the manager
	 *
	 * The system is constructed in within the manager and owned by it.
	 * Systems must derive from the System base class..
	 *
	 * @tparam T System type
	 * @tparam Args T Constructor argument types
	 * @param priority Execution priority (lower executes first)
	 * @param args System constructor arguments
	 *
	 * @return Reference to the created system
	 *
	 * Usage:
	 * @code
	 * PhysicsSystem& physics = systemManager.AddSystem<PhysicsSystem>(3, gravity);
	 * @endcode
	 */

	template <typename T, typename... Args>
		requires std::is_base_of_v<System, T>
	T& AddSystem(const u32 priority = 1, Args&&... args)
	{
		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
		T& ref = *ptr;

		m_systems.push_back(std::make_pair(priority, std::move(ptr)));

		std::sort(m_systems.begin(), m_systems.end(), [](const auto& a, const auto& b)
		{
			return a.first > b.first;
		});

		return ref;
	}

private:

	std::vector<std::pair<u32, std::unique_ptr<System>>> m_systems;
};