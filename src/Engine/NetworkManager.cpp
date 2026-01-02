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
	_thread.join();
}

PeerId NetworkManager::Connect(const Address& address, const u32 data) 
{
	_connectQueue.enqueue(std::make_pair(address, data));

	PeerId id;
	_connectReturnQueue.wait_dequeue(id);

	return id;
}

void NetworkManager::Disconnect(const PeerId peer, const u32 data) 
{
	_disconnectQueue.enqueue(std::make_pair(peer, data));
}

void NetworkManager::Send(const PeerId peer, const std::vector<u8>& data, const ChannelId channel, const bool reliable) 
{
	SendData sendData = {peer, std::move(data), channel, reliable};

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

Peer NetworkManager::GetPeer(const PeerId peer) 
{
	_peerIdQueue.enqueue(peer);

	Peer p;
	_peersQueue.wait_dequeue(p);

	return p;
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

		std::queue<NetworkEvent> events;
		_networkCore.Poll(events, timeoutMs);
		if (events.size())
		{
			_eventQueue.enqueue(events);
		}
	}
}