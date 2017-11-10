#include "heartbeat_module.h"
#include "winner_relay_app.h"

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_time.h>

using namespace TSystem;

HeartbeatModule::HeartbeatModule(WinnerRelayApp& app)
	: app_(app)
	, shutdown_(), strand_(app_.task_pool())
	, broadcast_timer_(app_.task_pool())
	, heartbeat_holder_()
	, heartbeat_map_(256)
{
	shutdown_ = false;
}

void HeartbeatModule::Initiate()
{
	// reserve space
	int n = 0;
	std::for_each(std::begin(app_.resource_manager().AllResources()), std::end(app_.resource_manager().AllResources())
		, [&n](const std::unique_ptr<layinfo::AppResource>& entry){ if( entry->port ) ++n; });
	heartbeat_holder_.reserve(n);

	// create pid heartbeat
	static unsigned short heartbeat_id = app_.msg_system().FindClassID("Heartbeat");
	std::for_each(std::begin(app_.resource_manager().AllResources()), std::end(app_.resource_manager().AllResources())
		, [this](const std::unique_ptr<layinfo::AppResource>& entry)
	{ 
		if( entry->type != "misc"
			&& entry->pid != app_.pid()
			&& entry->node == app_.node()
			&& entry->dev_tag == utility::DevTag() )
		{
			PidHeartbeat a;
			a.heartbeat.set_pid(entry->pid);
			a.topic_data = &app_.topic_server_.AddTopicData(heartbeat_id, utility::FormatStr("%d", entry->pid), app_.task_pool());
			heartbeat_holder_.push_back(std::move(a));
		}
	});

	// create map
	std::for_each(std::begin(heartbeat_holder_), std::end(heartbeat_holder_)
		,[this](PidHeartbeat& entry)
	{ heartbeat_map_.insert(std::make_pair(entry.heartbeat.pid(), &entry)); });

	// register handler
	app_.msg_handlers_.RegisterHandler("Heartbeat", [this](communication::Connection*, const Message& msg)
	{
		if( !shutdown_ )
		{
			if( app_.forward_module_.CheckForward(msg) )
				return;

			std::shared_ptr<Heartbeat> p = std::make_shared<Heartbeat>();
			if( Decode(msg, *p, app_.msg_system()) )
				strand_.PostTask(std::bind(&HeartbeatModule::UpdateHeartBeat, this, std::move(p)));
		}
	});
}

void HeartbeatModule::Broadcast()
{
	std::chrono::system_clock::time_point now = GetClock().Now();

	std::for_each(std::begin(heartbeat_holder_), std::end(heartbeat_holder_)
		, [this, &now](PidHeartbeat& entry)
	{
		// check miss heartbeat
		if( entry.heartbeat.has_timestamp() && entry.heartbeat.status() != Heartbeat::OFF_LINE )
		{
			std::chrono::system_clock::time_point pid_time = 
				MakeTimePoint(entry.heartbeat.timestamp().time_value(), entry.heartbeat.timestamp().frac_sec());
		
			if( now - pid_time > std::chrono::seconds(3) )
				entry.heartbeat.set_status(Heartbeat::OFF_LINE);
		}
		
		// update 
		communication::CodedDataPtr p = Encode(entry.heartbeat, app_.msg_system(), Message::HeaderType(0, app_.pid(), 0));
		entry.topic_data->AsyncUpdateData(p);
	});

	StartBroadcast();
}


void HeartbeatModule::UpdateHeartBeat(const std::shared_ptr<Heartbeat>& hb)
{
	auto iter = heartbeat_map_.find(hb->pid());
	if( iter == heartbeat_map_.end() )
	{
		LogError(LogMessage::VITAL, CoreErrorCategory::ErrorCode::BAD_CONTENT, "HeartbeatModule::UpdateHeartBeat"
			, utility::FormatStr("Can't find pid in Resource belonging to this node: %d", hb->pid()), app_.local_logger());
		return ;
	}

	FillTime(TimePoint(GetClock().Now()), *iter->second->heartbeat.mutable_timestamp());
	iter->second->heartbeat.set_status(hb->status());
}

void HeartbeatModule::StartBroadcast()
{
	// must run in strand
	auto HandleBroadcast = [this](const TSystem::TError& te)
	{
		if( !te )
			this->strand_.PostTask(std::bind(&HeartbeatModule::Broadcast, this));
	};

	if( !app_.HasShutdown() )
	{
		broadcast_timer_.ExpirationDuration(std::chrono::seconds(3));
		broadcast_timer_.PostTask(HandleBroadcast);
	}
}

void HeartbeatModule::Shutdown()
{
	shutdown_ = true;
	broadcast_timer_.Cancel();
}

void HeartbeatModule::Start()
{
	StartBroadcast();
}
