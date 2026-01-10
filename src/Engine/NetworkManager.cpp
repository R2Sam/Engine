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
	Assert(!_running, "Cannot start server when one is already running");

	_running = _networkCore.InitServer(port, maxPeers, channels);

	_thread = std::thread(&NetworkManager::Loop, this, timeoutMs);

	return _running;
}

bool NetworkManager::InitClient(const u32 channels, const u32 timeoutMs) 
{
	Assert(!_running, "Cannot start client when one is already running");

	_running = _networkCore.InitClient(channels);

	_thread = std::thread(&NetworkManager::Loop, this, timeoutMs);

	return _running;
}

void NetworkManager::Shutdown() 
{
	_running = false;

	if (_thread.joinable())
	{
		_thread.join();
	}
}

Peer NetworkManager::Connect(const Address& address, const u32 data) 
{
	_connectQueue.enqueue(std::make_pair(address, data));

	Peer peer;
	_connectReturnQueue.wait_dequeue(peer);

	return peer;
}

void NetworkManager::Disconnect(const PeerId peerId, const u32 data) 
{
	_disconnectQueue.enqueue(std::make_pair(peerId, data));
}

void NetworkManager::Send(const PeerId peerId, const std::vector<u8>& data, const ChannelId channel, const bool reliable) 
{
	SendData sendData = {peerId, std::move(data), channel, reliable};

	_sendQueue.enqueue(sendData);
}

std::queue<NetworkEvent> NetworkManager::Poll() 
{
	std::queue<NetworkEvent> queue;
	if (_eventQueue.try_dequeue(queue))
	{
		return queue;
	}

	return queue;
}

Peer NetworkManager::GetPeer(const PeerId peerId)
{
	_peerIdQueue.enqueue(peerId);

	Peer peer;
	_peersQueue.wait_dequeue(peer);

	return peer;
}

const std::unordered_map<PeerId, Peer>& NetworkManager::GetPeers() 
{
	_peerMapRequest = true;

	const std::unordered_map<PeerId, Peer>* map;
	_peerMapQueue.wait_dequeue(map);

	return *map;
}

void NetworkManager::Loop(const u32 timeoutMs) 
{
	while(_running)
	{
		std::pair<Address, u32> connectPair;
		if (_connectQueue.try_dequeue(connectPair))
		{
			_connectReturnQueue.enqueue(_networkCore.Connect(connectPair.first, connectPair.second));
		}

		SendData sendData;
		if (_sendQueue.try_dequeue(sendData))
		{
			_networkCore.Send(sendData.peer, sendData.data, sendData.channel, sendData.reliable);
		}

		std::pair<PeerId, u32> disconnectPair;
		if (_disconnectQueue.try_dequeue(disconnectPair))
		{
			_networkCore.Disconnect(disconnectPair.first, disconnectPair.second);
		}

		PeerId id;
		if (_peerIdQueue.try_dequeue(id))
		{
			_peersQueue.enqueue(_networkCore.GetPeer(id));
		}

		if (_peerMapRequest)
		{
			_peerMapRequest = false;

			_peerMapQueue.enqueue(&_networkCore.GetPeers());
		}

		std::queue<NetworkEvent> events;
		_networkCore.Poll(events, timeoutMs);
		if (events.size())
		{
			_eventQueue.enqueue(events);
		}
	}
}