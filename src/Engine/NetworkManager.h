#pragma once

#include <unordered_map>
#ifndef assert
#define assert(condition) ((void)0)
#endif

#include "Networking/NetworkCore.h"
#include "ThreadSafeQueue/blockingconcurrentqueue.h"
#include "ThreadSafeQueue/concurrentqueue.h"

class NetworkManager
{
public:

	NetworkManager();
	~NetworkManager();

	bool InitServer(const u16 port, const u32 maxPeers = 64, const u32 channels = 1, const u32 timeoutMs = 0);
	bool InitClient(const u32 channels = 1, const u32 timeoutMs = 0);
	void Shutdown();

	Peer Connect(const Address& address, const u32 data = 0);
	void Disconnect(const PeerId peerId, const u32 data = 0);

	void Send(const PeerId peerId, std::vector<u8>&& data, const ChannelId channel = 0, const bool reliable = true);

	std::queue<NetworkEvent> Poll();

	Peer GetPeer(const PeerId peerId);
	const std::unordered_map<PeerId, Peer>& GetPeers();

private:

	void Loop(const u32 timeoutMs = 0);

	struct SendData
	{
		PeerId peer;
		std::vector<u8> data;
		ChannelId channel;
		bool reliable;
	};

	NetworkCore m_networkCore;
	std::atomic<bool> m_running;

	std::thread m_thread;

	moodycamel::ConcurrentQueue<std::pair<Address, u32>> m_connectQueue;
	moodycamel::BlockingConcurrentQueue<Peer> m_connectReturnQueue;
	moodycamel::ConcurrentQueue<SendData> m_sendQueue;
	moodycamel::ConcurrentQueue<std::queue<NetworkEvent>> m_eventQueue;
	moodycamel::ConcurrentQueue<std::pair<PeerId, u32>> m_disconnectQueue;
	moodycamel::ConcurrentQueue<PeerId> m_peerIdQueue;
	moodycamel::BlockingConcurrentQueue<Peer> m_peersQueue;

	std::atomic<bool> m_peerMapRequest = false;
	moodycamel::BlockingConcurrentQueue<const std::unordered_map<PeerId, Peer>*> m_peerMapQueue;
};