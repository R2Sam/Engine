#pragma once

#include "Engine/Components.hpp"
#include "Engine/Registry.hpp"
#include "Engine/SystemManager.hpp"
#include "Networking/Encryption.hpp"
#include "Networking/NetworkTypes.hpp"
#include "cereal/MyCereal.h"
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cstddef>
#include <unordered_map>

using NetPeerId = UUID;

struct NetworkEntityUpdatePacket
{
	std::vector<std::byte> transform;

	UUID owner = NULL_UUID;
	Entity entity = NULL_ENTITY;

	template <class Archive>
	void serialize(Archive& archive) // NOLINT
	{
		archive(transform, owner, entity);
	}
};

/**
 * @class NetworkEntitySystem
 * @brief Maintains a mapping from (owner, remote entity) to local entity.
 *
 * Uses Component::NetworkId to associate a local entity with a remote
 * peer and its entity ID. The mapping is updated automatically via
 * OnConstruct / OnDestroy callbacks.
 */
class NetworkEntitySystem : public System
{
public:

	/**
	 * @brief Constructs the system and registers component callbacks.
	 */
	NetworkEntitySystem();

	/**
	 * @brief Update step (currently reserved for future network sync).
	 * @param deltaT Time since last update (unused).
	 */
	void Update(const float deltaT) override;

	/**
	 * @brief Retrieves the local entity for a given (owner, remoteEntity) pair.
	 * @param owner UUID of the peer that owns the remote entity.
	 * @param remoteEntity Entity ID on the remote side.
	 * @return The corresponding local Entity, or NULL_ENTITY if not found.
	 */
	Entity GetLocalEntity(const UUID& owner, const Entity remoteEntity) const;

	/**
	 * @brief Assigns a NetworkId component to an entity.
	 * @param entity The local entity to tag.
	 * @param owner UUID of the owning peer.
	 * @param remoteEntity Entity ID on the remote side.
	 *
	 * Convenience wrapper for REGISTRY.EmplaceOrReplace<Component::NetworkId>.
	 */
	static void AssignNetworkId(const Entity entity, const UUID& owner, const Entity remoteEntity);

	/**
	 * @brief Stores the list of currently connected peers.
	 * @param peers Vector of Peer structures.
	 *
	 * Used for future network filtering or broadcasting.
	 */
	void SetConnectedPeers(const std::vector<Peer>& peers);

	template <CerealSerializable Component>
	void NetworkEntityComponent(const Entity entity, const bool reliable)
	{
		auto it = m_networkedComponents.find(entity);
		if (it != m_networkedComponents.end())
		{
			auto it2 = it->second.find(typeid(Component));
			if (it2 != it->second.end())
			{
				it2->second = reliable;
				return;
			}

			it->second.emplace(typeid(Entity), reliable);
			return;
		}

		m_networkedComponents[entity].emplace(entity, reliable);

		m_registry.OnUpdate<Component>([this](Component& component, const Entity entity)
		{
			OnUpdate(component, entity);
		});
	}

private:

	void Broadcast(std::vector<std::byte>&& data, const bool reliable);

	void OnConstruct(Component::NetworkId& component, const Entity entity);

	template <typename Component>
	void OnUpdate(Component& component, const Entity entity)
	{
		auto it = m_networkedComponents.find(entity);
		if (it == m_networkedComponents.end())
		{
			return;
		}

		auto it2 = it->second.find(typeid(Component));
		if (it2 == it->second.end())
		{
			return;
		}

		for (UpdatePacket& packet : m_updates)
		{
			if (packet.type == typeid(Component))
			{
				packet.data = Serialize<Component>(component);
				return;
			}
		}

		m_updates.emplace_back(typeid(Component), true, Serialize<Component>(component));
	}

	void OnDestroy(Component::NetworkId& component, const Entity entity);

	struct Key
	{
		UUID owner;
		Entity remoteEntity;

		bool operator==(const Key& other) const = default;
	};

	struct KeyHash
	{
		u64 operator()(const Key& key) const noexcept
		{
			u64 hash = 0;
			for (const auto b : key.owner)
			{
				hash = (hash * 31) ^ b;
			}

			hash = (hash * 31) ^ static_cast<u32>(key.remoteEntity);
			return hash;
		}
	};

	struct UpdatePacket
	{
		std::type_index type;
		bool reliable = true;

		std::vector<std::byte> data;
	};

	std::unordered_map<Key, Entity, KeyHash> m_remoteToLocal;

	std::vector<Peer> m_connectedPeers;
	std::unordered_map<Entity, std::unordered_map<std::type_index, bool>> m_networkedComponents;
	std::vector<UpdatePacket> m_updates;

	Registry& m_registry;
};