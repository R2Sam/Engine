#pragma once

// Forward
#include <utility>
struct Context;

#include "Assert.h"
#include "Types.h"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <vector>

/**
 * @brief Base class for all systems
 *
 * All systems must derive from this class and implement Update() and Draw().
 * The first constructor parameter of any derived system must be a Context reference.
 */

class System
{
public:

	/**
	 * @brief Constructs the system with a shared context
	 *
	 * @param context Immutable reference to engine context
	 */

	System(const Context& context) :
	_context(context)
	{
	}

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

protected:

	const Context& _context;
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
	 * Systems must derive from the System base class.
	 * They must also accept a immutable reference to Context as first argument.
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
		Assert(_context, "Context must be set first");

		auto ptr = std::make_unique<T>(*_context, std::forward<Args>(args)...);
		T& ref = *ptr;

		_systems.push_back(std::make_pair(priority, std::move(ptr)));

		std::sort(_systems.begin(), _systems.end(), [](const auto& a, const auto& b)
		{
			return a.first > b.first;
		});

		return ref;
	}

	/**
	 * @brief Sets the shared context used by all systems
	 *
	 * Must be called after class construction
	 *
	 * @param context Immutable reference to engine context
	 */

	void SetContext(Context& context);

private:

	Context* _context = nullptr;

	std::vector<std::pair<u32, std::unique_ptr<System>>> _systems;
};