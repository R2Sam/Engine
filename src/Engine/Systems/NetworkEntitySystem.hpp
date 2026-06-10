#pragma once

#include "Assert.hpp"
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

/**
 * @file NetworkEntitySystem.hpp
 * @brief Network entity mapping and component synchronisation.
 */

/**
 * @struct NetworkEntityUpdatePacket
 * @brief Packet sent over the network to synchronise entity transforms.
 *
 * Carries the serialised transform data along with the owning peer and
 * the remote entity identifier. Used by the NetworkEntitySystem to broadcast
 * or receive transform updates.
 */
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
 * @brief Maintains a bidirectional mapping between (owner, remote entity) and local entities.
 *
 * Listens for `Component::NetworkId` construction/destruction and builds a lookup table
 * that translates a remote peer + remote entity ID into the corresponding local entity.
 * Also tracks which components of an entity should be networked and collects pending
 * updates for broadcast.
 */
class NetworkEntitySystem : public System
{
public:

	/**
	 * @brief Constructs the system and registers component callbacks.
	 *
	 * Subscribes to `OnConstruct` and `OnDestroy` events for `Component::NetworkId`
	 * to keep the internal mapping synchronised.
	 */
	NetworkEntitySystem();

	/**
	 * @brief Processes pending network updates and sends them.
	 * @param deltaT Time since last update (not used currently).
	 *
	 * Iterates over collected component updates, builds appropriate network packets
	 * (e.g., `NetworkEntityUpdatePacket` for transforms), and broadcasts them to
	 * all connected peers.
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

	/**
	 * @brief Marks a component type on an entity for network synchronisation.
	 * @tparam Component The component type (must be serializable via Cereal).
	 * @param entity The local entity whose component should be networked.
	 * @param reliable Whether updates for this component should use reliable delivery.
	 *
	 * When the component is updated (via `OnUpdate`), the system will collect its
	 * serialised data and send it automatically. The system internally registers
	 * an update listener for the component type.
	 */
	template <CerealSerializable ComponentT>
	void NetworkEntityComponent(const Entity entity, const bool reliable)
	{
		Assert(m_registry.EntityValid(entity));

		const auto* networkId = m_registry.Get<Component::NetworkId>(entity);
		Assert(networkId, "Can't network a component without a network id");
		// TODO
		// Check that it is ours to actually update

		auto it = m_networkedComponents.find(entity);
		if (it != m_networkedComponents.end())
		{
			auto it2 = it->second.find(typeid(ComponentT));
			if (it2 != it->second.end())
			{
				it2->second = reliable;
				return;
			}

			it->second.emplace(typeid(Entity), reliable);
			return;
		}

		m_networkedComponents[entity].emplace(typeid(ComponentT), reliable);

		m_registry.OnUpdate<ComponentT>([this](ComponentT& component, const Entity entity)
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