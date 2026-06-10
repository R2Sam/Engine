#include "NetworkEntitySystem.hpp"
#include "Engine/Components.hpp"
#include "Engine/Engine.hpp"
#include "Log/Log.hpp"
#include "cereal/MyCereal.h"

NetworkEntitySystem::NetworkEntitySystem() :
m_registry(REGISTRY)
{
	REGISTRY.OnConstruct<Component::NetworkId>([this](Component::NetworkId& networkId, const Entity entity)
	{
		OnConstruct(networkId, entity);
	});

	REGISTRY.OnDestroy<Component::NetworkId>([this](Component::NetworkId& networkId, const Entity entity)
	{
		OnDestroy(networkId, entity);
	});
}

void NetworkEntitySystem::Update([[maybe_unused]] const float deltaT)
{
	for (UpdatePacket& updatePacket : m_updates)
	{
		if (Demangle(updatePacket.type) == Demangle<Component::Transform>())
		{
			NetworkEntityUpdatePacket packet = {updatePacket.data}; // TODO

			Broadcast(Serialize(packet), updatePacket.reliable);

			continue;
		}
	}

	m_updates.clear();
}

Entity NetworkEntitySystem::GetLocalEntity(const UUID& owner, const Entity remoteEntity) const
{
	Key key = {owner, remoteEntity};
	auto it = m_remoteToLocal.find(key);

	if (it != m_remoteToLocal.end())
	{
		return it->second;
	}

	return NULL_ENTITY;
}

void NetworkEntitySystem::AssignNetworkId(const Entity entity, const UUID& owner, const Entity remoteEntity)
{
	REGISTRY.EmplaceOrReplace<Component::NetworkId>(entity, owner, remoteEntity);
}

void NetworkEntitySystem::Broadcast(std::vector<std::byte>&& data, const bool reliable)
{
	for (const Peer& peer : m_connectedPeers)
	{
		NETWORK.Send(peer.id, std::move(data), 0, reliable);
	}
}

void NetworkEntitySystem::OnConstruct(Component::NetworkId& component, const Entity entity)
{
	Key key = {component.owner, component.remoteEntity};
	m_remoteToLocal[key] = entity;
}

void NetworkEntitySystem::OnDestroy(Component::NetworkId& component, const Entity entity)
{
	Key key = {component.owner, component.remoteEntity};
	auto it = m_remoteToLocal.find(key);
	if (it != m_remoteToLocal.end() && it->second == entity)
	{
		m_remoteToLocal.erase(it);
	}
}