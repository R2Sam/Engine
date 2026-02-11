#include "NetworkManager.h"

#include "Assert.h"

NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
	Shutdown();
}

bool NetworkManager::InitServer(const u16 port, const u32 maxPeers, const u32 channels, const u32 timeoutMs)
{
	Assert(!m_running, "Cannot start server when one is already running");

	m_running = m_networkCore.InitServer(port, maxPeers, channels);

	m_thread = std::thread(&NetworkManager::Loop, this, timeoutMs);

	return m_running;
}

bool NetworkManager::InitClient(const u32 channels, const u32 timeoutMs)
{
	Assert(!m_running, "Cannot start client when one is already running");

	m_running = m_networkCore.InitClient(channels);

	m_thread = std::thread(&NetworkManager::Loop, this, timeoutMs);

	return m_running;
}

void NetworkManager::Shutdown()
{
	m_running = false;

	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

Peer NetworkManager::Connect(const Address& address, const u32 data)
{
	m_connectQueue.enqueue(std::make_pair(address, data));

	Peer peer;
	m_connectReturnQueue.wait_dequeue(peer);

	return peer;
}

void NetworkManager::Disconnect(const PeerId peerId, const u32 data)
{
	m_disconnectQueue.enqueue(std::make_pair(peerId, data));
}

void NetworkManager::Send(const PeerId peerId, std::vector<u8>& data, const ChannelId channel, const bool reliable)
{
	SendData sendData = {peerId, std::move(data), channel, reliable};

	m_sendQueue.enqueue(sendData);
}

std::queue<NetworkEvent> NetworkManager::Poll()
{
	std::queue<NetworkEvent> queue;
	if (m_eventQueue.try_dequeue(queue))
	{
		return queue;
	}

	return queue;
}

Peer NetworkManager::GetPeer(const PeerId peerId)
{
	m_peerIdQueue.enqueue(peerId);

	Peer peer;
	m_peersQueue.wait_dequeue(peer);

	return peer;
}

const std::unordered_map<PeerId, Peer>& NetworkManager::GetPeers()
{
	m_peerMapRequest = true;

	const std::unordered_map<PeerId, Peer>* map;
	m_peerMapQueue.wait_dequeue(map);

	return *map;
}

void NetworkManager::Loop(const u32 timeoutMs)
{
	while (m_running)
	{
		std::pair<Address, u32> connectPair;
		if (m_connectQueue.try_dequeue(connectPair))
		{
			m_connectReturnQueue.enqueue(m_networkCore.Connect(connectPair.first, connectPair.second));
		}

		SendData sendData;
		if (m_sendQueue.try_dequeue(sendData))
		{
			m_networkCore.Send(sendData.peer, sendData.data, sendData.channel, sendData.reliable);
		}

		std::pair<PeerId, u32> disconnectPair;
		if (m_disconnectQueue.try_dequeue(disconnectPair))
		{
			m_networkCore.Disconnect(disconnectPair.first, disconnectPair.second);
		}

		PeerId id;
		if (m_peerIdQueue.try_dequeue(id))
		{
			m_peersQueue.enqueue(m_networkCore.GetPeer(id));
		}

		if (m_peerMapRequest)
		{
			m_peerMapRequest = false;

			m_peerMapQueue.enqueue(&m_networkCore.GetPeers());
		}

		std::queue<NetworkEvent> events;
		m_networkCore.Poll(events, timeoutMs);
		if (events.empty())
		{
			m_eventQueue.enqueue(events);
		}
	}
}