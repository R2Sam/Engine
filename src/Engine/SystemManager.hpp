#pragma once

#include "NonCopyable.hpp"
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
 * @file SystemManager.hpp
 * @brief Engine lifelong systems and their managing.
 */

/**
 * @brief Base class for all systems
 *
 * All systems must derive from this class and implement Update.
 */
class System : public NonCopyable<>
{
public:

	virtual ~System() = default;

	/**
	 * @brief Updates the system
	 *
	 * Called once per fixed-timestep step before rendering, in priority order.
	 * Called before scenes.
	 *
	 * @param deltaT Duration of the previous frame in seconds
	 */
	virtual void Update(const float deltaT) = 0;

	/**
	 * @brief Renders the system
	 *
	 * Called once per frame after all Update steps, in priority order.
	 * Called before scenes. Default implementation does nothing.
	 */
	virtual void Draw() const;
};

/**
 * @brief Manages execution order and lifetime of systems
 *
 * Systems are updated and drawn in ascending priority order.
 * Lower priority values execute first.
 * Systems update and draw before scenes each frame.
 */
class SystemManager
{
public:

	SystemManager() = default;

	SystemManager(const SystemManager&) = delete;
	SystemManager& operator=(const SystemManager&) = delete;

	/**
	 * @brief Constructs a system and adds it to the manager
	 *
	 * The system is owned by the manager. Systems are sorted by priority
	 * immediately after insertion.
	 *
	 * @tparam SystemT System type (must derive from the System base class)
	 * @tparam Args System constructor argument types
	 * @param priority Execution priority — lower values run first (default 1)
	 * @param args Arguments forwarded to the system constructor
	 * @return Shared pointer to the created system
	 *
	 * Usage:
	 * @code
	 * auto physics = systemManager.AddSystem<PhysicsSystem>(3, gravity);
	 * @endcode
	 */
	template <typename SystemT, typename... Args>
		requires std::is_base_of_v<System, SystemT>
	std::shared_ptr<SystemT> AddSystem(const u32 priority = 1, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		auto ptr = std::make_shared<SystemT>(std::forward<Args>(args)...);

		m_systems.push_back(std::make_pair(priority, ptr));
		m_systemsMap.emplace(typeid(SystemT), ptr);

		std::sort(m_systems.begin(), m_systems.end(), [](const auto& a, const auto& b)
		{
			return a.first < b.first;
		});

		return ptr;
	}

	/**
	 * @brief Removes a system from the manager
	 *
	 * The system is destroyed when no more shared_ptrs to it remain.
	 * Does nothing if the system is not currently managed.
	 *
	 * @tparam SystemT System type to remove
	 */
	template <typename SystemT>
		requires std::is_base_of_v<System, SystemT>
	void RemoveSystem()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_systemsMap.find(typeid(SystemT));
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
	 * @brief Returns a shared pointer to a managed system
	 *
	 * Returns an empty pointer if no system of this type has been added.
	 *
	 * @tparam SystemT System type to retrieve
	 * @return Shared pointer to the system, or nullptr if not found
	 */
	template <typename SystemT>
		requires std::is_base_of_v<System, SystemT>
	std::shared_ptr<SystemT> GetSystem()
	{
		std::shared_lock lock(m_mutex);

		auto it = m_systemsMap.find(typeid(SystemT));
		if (it != m_systemsMap.end())
		{
			return std::dynamic_pointer_cast<SystemT>(it->second);
		}

		return nullptr;
	}

	/**
	 * @brief Removes and destroys all managed systems
	 *
	 * Called by the Engine destructor.
	 */
	void ClearSystems();

private:

	/**
	 * @brief Calls Update on all systems in priority order
	 *
	 * @param deltaT Duration of the previous frame in seconds
	 */
	void Update(const float deltaT);

	/**
	 * @brief Calls Draw on all systems in priority order
	 */
	void Draw();

	std::shared_mutex m_mutex;

	std::vector<std::pair<u32, std::shared_ptr<System>>> m_systems;
	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systemsMap;

	friend class Engine;
};