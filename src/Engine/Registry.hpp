#pragma once

#include "Assert.hpp"
#include "entt/entt.hpp"
#include <Types.hpp>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using Entity = entt::entity;

constexpr Entity NULL_ENTITY = static_cast<Entity>(0);

/**
 * @file Registry.hpp
 * @brief Engine entt::registry wrapper.
 */

/**
 * @brief Wraps entt::registry with a safer, callback-aware API
 *
 * Provides typed component operations and supports registering callbacks
 * for component construction, update, and destruction events.
 * All callbacks are identified by a u32 ID so they can be individually removed.
 */
class Registry
{
public:

	/**
	 * @brief Creates a new entity
	 *
	 * @return The newly created entity
	 */
	Entity CreateEntity();

	/**
	 * @brief Destroys an entity if it is valid
	 *
	 * Does nothing if the entity is not valid.
	 *
	 * @param entity Entity to destroy
	 */
	void DestroyEntity(const Entity entity);

	/**
	 * @brief Checks whether an entity is alive in the registry
	 *
	 * @param entity Entity to check
	 * @return True if the entity is valid
	 */
	bool EntityValid(const Entity entity);

	/**
	 * @brief Adds a component to an entity
	 *
	 * Does nothing and returns nullptr if the entity already has this component.
	 *
	 * @tparam Component Component type to add
	 * @tparam Args Component constructor argument types
	 * @param entity Target entity
	 * @param args Component constructor arguments
	 * @return Pointer to the new component, or nullptr if it already existed
	 */
	template <typename Component, typename... Args>
	const Component* Emplace(const Entity entity, Args&&... args)
	{
		Assert(EntityValid(entity));
		if (HasAny<Component>(entity))
		{
			return nullptr;
		}

		return &m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
	}

	/**
	 * @brief Adds or replaces a component on an entity
	 *
	 * If the component already exists it is replaced in place; otherwise it is created.
	 *
	 * @tparam Component Component type
	 * @tparam Args Component constructor argument types
	 * @param entity Target entity
	 * @param args Component constructor arguments
	 * @return Reference to the component
	 */
	template <typename Component, typename... Args>
	const Component& EmplaceOrReplace(const Entity entity, Args&&... args)
	{
		Assert(EntityValid(entity));
		return m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
	}

	/**
	 * @brief Applies a function to an existing component in place
	 *
	 * Triggers update callbacks. Does nothing and returns false if the entity
	 * does not have the component.
	 *
	 * @tparam Component Component type
	 * @param entity Target entity
	 * @param func Function to apply to the component
	 * @return True if the component existed and was patched
	 */
	template <typename Component>
	bool Patch(const Entity entity, const std::function<void(Component&)>& func)
	{
		Assert(EntityValid(entity));
		if (!m_registry.any_of<Component>(entity))
		{
			return false;
		}

		m_registry.patch<Component>(entity, func);

		return true;
	}

	/**
	 * @brief Gets a pointer to a component on an entity
	 *
	 * @tparam Component Component type
	 * @param entity Target entity
	 * @return Pointer to the component, or nullptr if the entity does not have it
	 */
	template <typename Component>
	const Component* Get(const Entity entity)
	{
		Assert(EntityValid(entity));
		Component* ptr = m_registry.try_get<Component>(entity);
		if (!ptr)
		{
			return nullptr;
		}

		return ptr;
	}

	/**
	 * @brief Checks whether an entity has at least one of the given components
	 *
	 * @tparam Components Component types to check
	 * @param entity Target entity
	 * @return True if the entity has any of the specified components
	 */
	template <typename... Components>
	bool HasAny(const Entity entity)
	{
		Assert(EntityValid(entity));
		return m_registry.any_of<Components...>(entity);
	}

	/**
	 * @brief Checks whether an entity has all of the given components
	 *
	 * @tparam Components Component types to check
	 * @param entity Target entity
	 * @return True if the entity has every specified component
	 */
	template <typename... Components>
	bool HasAll(const Entity entity)
	{
		Assert(EntityValid(entity));
		return m_registry.all_of<Components...>(entity);
	}

	/**
	 * @brief Replaces an existing component on an entity
	 *
	 * Does nothing and returns nullptr if the entity does not have the component.
	 *
	 * @tparam Component Component type
	 * @tparam Args Component constructor argument types
	 * @param entity Target entity
	 * @param args Component constructor arguments
	 * @return Pointer to the replaced component, or nullptr if it did not exist
	 */
	template <typename Component, typename... Args>
	const Component* Replace(const Entity entity, Args&&... args)
	{
		Assert(EntityValid(entity));
		if (!HasAny<Component>(entity))
		{
			return nullptr;
		}

		return &m_registry.replace<Component>(entity, std::forward<Args>(args)...);
	}

	/**
	 * @brief Removes a component from an entity
	 *
	 * Does nothing if the entity does not have the component.
	 *
	 * @tparam Component Component type to remove
	 * @param entity Target entity
	 */
	template <typename Component>
	void Remove(const Entity entity)
	{
		Assert(EntityValid(entity));
		m_registry.remove<Component>(entity);
	}

	/**
	 * @brief Removes all instances of a component type from every entity
	 *
	 * @tparam Component Component type to clear
	 */
	template <typename Component>
	void ClearComponents()
	{
		m_registry.clear<Component>();
	}

	/**
	 * @brief Returns a view over all entities that have the given components
	 *
	 * Components are exposed as const references inside the view.
	 *
	 * @tparam Components Component types to include in the view
	 * @return entt view over the matching entities
	 */
	template <typename... Components>
	auto GetView()
	{
		return m_registry.view<const Components...>();
	}

	/**
	 * @brief Returns a group over entities with the specified owned and non-owned components
	 *
	 * Owned components are stored contiguously for cache-friendly iteration.
	 * Non-owned components are fetched via Get. Excluded components filter entities out.
	 *
	 * @tparam Owned Component types owned (and sorted) by the group
	 * @tparam Get Additional component types to fetch but not own
	 * @tparam Exclude Component types that disqualify an entity from the group
	 * @param get entt::get_t list of non-owned components to include
	 * @param exclude entt::exclude_t list of components to exclude
	 * @return entt group over the matching entities
	 */
	template <typename... Owned, typename... Get, typename... Exclude>
	auto GetGroup([[maybe_unused]] entt::get_t<Get...> get = entt::get_t{},
	[[maybe_unused]] entt::exclude_t<Exclude...> exclude = entt::exclude_t{})
	{
		return m_registry.group<const Owned...>(entt::get_t<const Get...>{}, entt::exclude_t<Exclude...>{});
	}

	/**
	 * @brief Sorts a component pool using a comparator
	 *
	 * The sort order affects the iteration order of views over this component.
	 *
	 * @tparam Component Component type whose pool is sorted
	 * @param comparitor Comparator receiving two const component references
	 */
	template <typename Component>
	void Sort(const std::function<bool(const Component& a, const Component& b)>& comparitor)
	{
		m_registry.sort<Component>(comparitor);
	}

	/**
	 * @brief Sorts a group's entities using an entity comparator
	 *
	 * Affects the iteration order of the group. The group is identified by the
	 * same owned/get/exclude template parameters used to create it.
	 *
	 * @tparam Owned Owned component types of the group
	 * @tparam Get Non-owned component types of the group
	 * @tparam Exclude Excluded component types of the group
	 * @param comparitor Comparator receiving two entity references
	 * @param get entt::get_t matching the group's get list
	 * @param exclude entt::exclude_t matching the group's exclude list
	 */
	template <typename... Owned, typename... Get, typename... Exclude>
	void SortGroup(const std::function<bool(const Entity& a, const Entity& b)>& comparitor,
	entt::get_t<Get...> get = entt::get_t{}, entt::exclude_t<Exclude...> exclude = entt::exclude_t{})
	{
		auto group = m_registry.group<Owned...>(get, exclude);
		group.sort(comparitor);
	}

	/**
	 * @brief Registers a callback invoked when a component of the given type is constructed
	 *
	 * Multiple callbacks can be registered for the same component type.
	 * The returned ID can be used with RemoveConstructCallback to unregister.
	 *
	 * @tparam Component Component type to observe
	 * @param callback Function receiving the new component and the owning entity
	 * @return Callback ID
	 */
	template <typename Component>
	u32 OnConstruct(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_callbackId++;

		auto& callbacks = m_constructCallbacks[typeid(Component)];
		callbacks[m_callbackId] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		if (!m_constructConnected.contains(typeid(Component)))
		{
			m_registry.on_construct<Component>().template connect<&Registry::HandleConstruct<Component>>(this);
			m_constructConnected.insert(typeid(Component));
		}

		return m_callbackId;
	};

	/**
	 * @brief Registers a callback invoked when a component of the given type is updated
	 *
	 * Triggered by Patch and Replace. Multiple callbacks can be registered for the
	 * same component type. The returned ID can be used with RemoveUpdateCallback.
	 *
	 * @tparam Component Component type to observe
	 * @param callback Function receiving the updated component and the owning entity
	 * @return Callback ID
	 */
	template <typename Component>
	u32 OnUpdate(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_callbackId++;

		auto& callbacks = m_updateCallbacks[typeid(Component)];
		callbacks[m_callbackId] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		if (!m_updateConnected.contains(typeid(Component)))
		{
			m_registry.on_update<Component>().template connect<&Registry::HandleUpdate<Component>>(this);
			m_updateConnected.insert(typeid(Component));
		}

		return m_callbackId;
	};

	/**
	 * @brief Registers a callback invoked just before a component of the given type is destroyed
	 *
	 * Multiple callbacks can be registered for the same component type.
	 * The returned ID can be used with RemoveDestroyCallback.
	 *
	 * @tparam Component Component type to observe
	 * @param callback Function receiving the component about to be destroyed and its entity
	 * @return Callback ID
	 */
	template <typename Component>
	u32 OnDestroy(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_callbackId++;

		auto& callbacks = m_destroyCallbacks[typeid(Component)];
		callbacks[m_callbackId] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		if (!m_destroyConnected.contains(typeid(Component)))
		{
			m_registry.on_destroy<Component>().template connect<&Registry::HandleDestroy<Component>>(this);
			m_destroyConnected.insert(typeid(Component));
		}

		return m_callbackId;
	};

	/**
	 * @brief Removes a previously registered construction callback
	 *
	 * Disconnects the underlying signal if no more callbacks remain for this type.
	 *
	 * @tparam Component Component type the callback was registered for
	 * @param callbackId ID returned by OnConstruct
	 */
	template <typename Component>
	void RemoveConstructCallback(const u32 callbackId)
	{
		auto it = m_constructCallbacks.find(typeid(Component));
		if (it == m_constructCallbacks.end())
		{
			return;
		}

		auto& map = it->second;
		map.erase(callbackId);

		if (map.empty())
		{
			m_registry.on_construct<Component>().template disconnect<&Registry::HandleConstruct<Component>>(this);
			m_constructConnected.erase(typeid(Component));
		}
	}

	/**
	 * @brief Removes a previously registered update callback
	 *
	 * Disconnects the underlying signal if no more callbacks remain for this type.
	 *
	 * @tparam Component Component type the callback was registered for
	 * @param callbackId ID returned by OnUpdate
	 */
	template <typename Component>
	void RemoveUpdateCallback(const u32 callbackId)
	{
		auto it = m_updateCallbacks.find(typeid(Component));
		if (it == m_updateCallbacks.end())
		{
			return;
		}

		auto& map = it->second;
		map.erase(callbackId);

		if (map.empty())
		{
			m_registry.on_update<Component>().template disconnect<&Registry::HandleUpdate<Component>>(this);
			m_updateConnected.erase(typeid(Component));
		}
	}

	/**
	 * @brief Removes a previously registered destruction callback
	 *
	 * Disconnects the underlying signal if no more callbacks remain for this type.
	 *
	 * @tparam Component Component type the callback was registered for
	 * @param callbackId ID returned by OnDestroy
	 */
	template <typename Component>
	void RemoveDestroyCallback(const u32 callbackId)
	{
		auto it = m_destroyCallbacks.find(typeid(Component));
		if (it == m_destroyCallbacks.end())
		{
			return;
		}

		auto& map = it->second;
		map.erase(callbackId);

		if (map.empty())
		{
			m_registry.on_destroy<Component>().template disconnect<&Registry::HandleDestroy<Component>>(this);
			m_destroyConnected.erase(typeid(Component));
		}
	}

	/**
	 * @brief Returns a reference to the underlying entt registry
	 *
	 * Prefer the typed API above. This escape hatch exists for interop with
	 * entt APIs not yet wrapped by Registry.
	 *
	 * @return Reference to the raw entt::registry
	 */
	entt::registry& GetRegistry();

private:

	template <typename Component>
	void HandleConstruct(entt::registry& registry, Entity entity)
	{
		auto it = m_constructCallbacks.find(typeid(Component));
		if (it == m_constructCallbacks.end())
		{
			return;
		}

		for (auto& pair : it->second)
		{
			pair.second(static_cast<void*>(&registry.get<Component>(entity)), entity);
		}
	}

	template <typename Component>
	void HandleUpdate(entt::registry& registry, Entity entity)
	{
		auto it = m_updateCallbacks.find(typeid(Component));
		if (it == m_updateCallbacks.end())
		{
			return;
		}

		for (auto& pair : it->second)
		{
			pair.second(static_cast<void*>(&registry.get<Component>(entity)), entity);
		}
	}

	template <typename Component>
	void HandleDestroy(entt::registry& registry, Entity entity)
	{
		auto it = m_destroyCallbacks.find(typeid(Component));
		if (it == m_destroyCallbacks.end())
		{
			return;
		}

		for (auto& pair : it->second)
		{
			pair.second(static_cast<void*>(&registry.get<Component>(entity)), entity);
		}
	}

	u32 m_callbackId = 0;

	std::unordered_map<std::type_index, std::unordered_map<u32, std::function<void(void*, const Entity entity)>>>
	m_constructCallbacks;
	std::unordered_map<std::type_index, std::unordered_map<u32, std::function<void(void*, const Entity entity)>>>
	m_updateCallbacks;
	std::unordered_map<std::type_index, std::unordered_map<u32, std::function<void(void*, const Entity entity)>>>
	m_destroyCallbacks;

	std::unordered_set<std::type_index> m_constructConnected;
	std::unordered_set<std::type_index> m_updateConnected;
	std::unordered_set<std::type_index> m_destroyConnected;

	entt::registry m_registry;
};