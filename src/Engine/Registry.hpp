#pragma once

#include "Assert.hpp"
#include "Types.hpp"
#include "entt/entt.hpp"
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>

using Entity = entt::entity;

class Registry
{
public:

	Entity CreateEntity();
	void DestroyEntity(const Entity entity);
	bool EntityValid(const Entity entity);

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

	template <typename Component, typename... Args>
	const Component& EmplaceOrReplace(const Entity entity, Args&&... args)
	{
		Assert(EntityValid(entity));
		return m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
	}

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

	template <typename... Components>
	bool HasAny(const Entity entity)
	{
		Assert(EntityValid(entity));
		return m_registry.any_of<Components...>(entity);
	}

	template <typename... Components>
	bool HasAll(const Entity entity)
	{
		Assert(EntityValid(entity));
		return m_registry.all_of<Components...>(entity);
	}

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

	template <typename Component>
	void Remove(const Entity entity)
	{
		Assert(EntityValid(entity));
		m_registry.remove<Component>(entity);
	}

	template <typename Component>
	void ClearComponents()
	{
		m_registry.clear<Component>();
	}

	template <typename... Components>
	auto GetView()
	{
		return m_registry.view<const Components...>();
	}

	template <typename... Owned, typename... Get, typename... Exclude>
	auto GetGroup([[maybe_unused]] entt::get_t<Get...> get = entt::get_t{},
	[[maybe_unused]] entt::exclude_t<Exclude...> exclude = entt::exclude_t{})
	{
		return m_registry.group<const Owned...>(entt::get_t<const Get...>{}, entt::exclude_t<Exclude...>{});
	}

	template <typename Component>
	void Sort(const std::function<bool(const Component& a, const Component& b)>& comparitor)
	{
		m_registry.sort<Component>(comparitor);
	}

	template <typename... Owned, typename... Get, typename... Exclude>
	void SortGroup(const std::function<bool(const Entity& a, const Entity& b)>& comparitor,
	entt::get_t<Get...> get = entt::get_t{}, entt::exclude_t<Exclude...> exclude = entt::exclude_t{})
	{
		auto group = m_registry.group<Owned...>(get, exclude);
		group.sort(comparitor);
	}

	template <typename Component>
	void OnConstruct(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_constructCallbacks[typeid(Component)] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		m_registry.on_construct<Component>().template connect<&Registry::HandleConstruct<Component>>(this);
		;
	};

	template <typename Component>
	void OnUpdate(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_updateCallbacks[typeid(Component)] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		m_registry.on_update<Component>().template connect<&Registry::HandleUpdate<Component>>(this);
		;
	};

	template <typename Component>
	void OnDestroy(const std::function<void(Component& component, const Entity entity)>& callback)
	{
		m_destroyCallbacks[typeid(Component)] = [callback](void* comp, const Entity entity)
		{
			callback(*static_cast<Component*>(comp), entity);
		};

		m_registry.on_destroy<Component>().template connect<&Registry::HandleDestroy<Component>>(this);
		;
	};

	entt::registry& GetRegistry();

private:

	template <typename Component>
	void HandleConstruct(entt::registry& registry, Entity entity)
	{
		m_constructCallbacks[typeid(Component)](static_cast<void*>(&registry.get<Component>(entity)), entity);
	}

	template <typename Component>
	void HandleUpdate(entt::registry& registry, Entity entity)
	{
		m_updateCallbacks[typeid(Component)](static_cast<void*>(&registry.get<Component>(entity)), entity);
	}

	template <typename Component>
	void HandleDestroy(entt::registry& registry, Entity entity)
	{
		m_destroyCallbacks[typeid(Component)](static_cast<void*>(&registry.get<Component>(entity)), entity);
	}

	std::unordered_map<std::type_index, std::function<void(void*, const Entity entity)>> m_constructCallbacks;
	std::unordered_map<std::type_index, std::function<void(void*, const Entity entity)>> m_updateCallbacks;
	std::unordered_map<std::type_index, std::function<void(void*, const Entity entity)>> m_destroyCallbacks;

	entt::registry m_registry;
};