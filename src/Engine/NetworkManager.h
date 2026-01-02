#pragma once

#include "Networking/NetworkCore.h"
#include "ThreadSafeQueue/concurrentqueue.h"
#include "ThreadSafeQueue/blockingconcurrentqueue.h"

class NetworkManager
{
public:

	NetworkManager();
	~NetworkManager();

	bool InitServer(const u16 port, const u32 maxPeers = 64, const u32 channels = 1, const u32 timeoutMs = 0);
	bool InitClient(const u32 channels = 1, const u32 timeoutMs = 0);
	void Shutdown();

	PeerId Connect(const Address& address, const u32 data = 0);
	void Disconnect(const PeerId peer, const u32 data = 0);

	void Send(const PeerId peer, const std::vector<u8>& data, const ChannelId channel = 0, const bool reliable = true);

	std::queue<NetworkEvent> Poll();

	Peer GetPeer(const PeerId peer);

private:

	void Loop(const u32 timeoutMs = 0);

private:

	struct SendData
	{
		PeerId peer;
		std::vector<u8> data;
		ChannelId channel;
		bool reliable;
	};

private:

	NetworkCore _networkCore;
	std::atomic<bool> _running;

	std::thread _thread;

	moodycamel::ConcurrentQueue<std::pair<Address, u32>> _connectQueue;
	moodycamel::BlockingConcurrentQueue<PeerId> _connectReturnQueue;
	moodycamel::ConcurrentQueue<SendData> _sendQueue;
	moodycamel::ConcurrentQueue<std::queue<NetworkEvent>> _eventQueue;
	moodycamel::ConcurrentQueue<std::pair<PeerId, u32>> _disconnectQueue;
	moodycamel::ConcurrentQueue<PeerId> _peerIdQueue;
	moodycamel::BlockingConcurrentQueue<Peer> _peersQueue;
};