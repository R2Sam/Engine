#include "Registry.hpp"

Entity Registry::CreateEntity()
{
	return m_registry.create();
}

void Registry::DestroyEntity(const Entity entity)
{
	m_registry.destroy(entity);
}

bool Registry::EntityValid(const Entity entity)
{
	return m_registry.valid(entity);
}

entt::registry& Registry::GetRegistry()
{
	return m_registry;
}