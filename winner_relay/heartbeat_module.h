#ifndef HEARTBEAT_MODULE_H_
#define HEARTBEAT_MODULE_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include <TLib/core/tsystem_task_service.h>
#include <TLib/core/tsystem_topic_service.h>
#include <TLib/tool/tsystem_heartbeat_msg.pb.h>

class WinnerRelayApp;

class HeartbeatModule
{
public:

	explicit HeartbeatModule( WinnerRelayApp& app);

	void Initiate();
	void Shutdown();
	void Start();

private:

	struct PidHeartbeat
	{
		PidHeartbeat(): heartbeat(), topic_data(nullptr){}

		PidHeartbeat(PidHeartbeat&& in): heartbeat(), topic_data(in.topic_data)
		{ heartbeat.Swap(&in.heartbeat); }

		Heartbeat								heartbeat;
		TSystem::communication::TopicData*		topic_data;
	};

	void StartBroadcast();

	void Broadcast();

	void UpdateHeartBeat(const std::shared_ptr<Heartbeat>& hb);

	WinnerRelayApp&			app_;

	std::atomic_bool        shutdown_;
	
	TSystem::TaskStrand		strand_;

	TSystem::TaskTimer      broadcast_timer_;

	std::vector<PidHeartbeat> heartbeat_holder_;

	std::unordered_map<unsigned short, PidHeartbeat*> heartbeat_map_;
};

#endif // HEARTBEAT_MODULE_H_