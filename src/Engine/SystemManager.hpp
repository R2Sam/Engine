#pragma once

#include "Types.hpp"

#include <algorithm>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief Base class for all systems
 *
 * All systems must derive from this class and implement Update.
 */

class System
{
public:

	System() = default;

	System(const System&) = delete;
	System& operator=(const System&) = delete;

	virtual ~System() = default;

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

	virtual void Draw();
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

	SystemManager() = default;

	SystemManager(const SystemManager&) = delete;
	SystemManager& operator=(const SystemManager&) = delete;

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
	 * @return Shared ptr to the created system
	 *
	 * Usage:
	 * @code
	 * auto physics = systemManager.AddSystem<PhysicsSystem>(3, gravity);
	 * @endcode
	 */

	template <typename T, typename... Args>
		requires std::is_base_of_v<System, T>
	std::shared_ptr<T> AddSystem(const u32 priority = 1, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		auto ptr = std::make_shared<T>(std::forward<Args>(args)...);

		m_systems.push_back(std::make_pair(priority, ptr));
		m_systemsMap.emplace(typeid(T), ptr);

		std::sort(m_systems.begin(), m_systems.end(), [](const auto& a, const auto& b)
		{
			return a.first < b.first;
		});

		return ptr;
	}

	template <typename T>
		requires std::is_base_of_v<System, T>
	void RemoveSystem()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_systemsMap.find(typeid(T));
		if (it == m_systemsMap.end())
		{
			return;
		}

		std::shared_ptr<System> ptr = it->second;
		i32 index = -1;

		for (u32 i = 0; i < m_systems.size(); ++i)
		{
			if (m_systems[i].second == ptr)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			m_systems.erase(m_systems.begin() + index);
		}
	}

	/**
	 * @brief Gets a system pointer
	 *
	 * Return a empty pointer if the system does not exist.
	 *
	 * @tparam T System type
	 */

	template <typename T>
		requires std::is_base_of_v<System, T>
	std::shared_ptr<T> GetSystem()
	{
		std::shared_lock lock(m_mutex);

		auto it = m_systemsMap.find(typeid(T));
		if (it != m_systemsMap.end())
		{
			return it->second;
		}

		return {};
	}

	/**
	 * @brief Clears all systems
	 */

	void ClearSystems();

private:

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

	std::shared_mutex m_mutex;

	std::vector<std::pair<u32, std::shared_ptr<System>>> m_systems;
	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systemsMap;

	friend class Engine;
};