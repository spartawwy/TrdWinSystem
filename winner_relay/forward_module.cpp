#include "forward_module.h"
#include "winner_relay_app.h"

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>

using namespace TSystem;

ForwardModule::ForwardModule(WinnerRelayApp& app)
	: app_(app)
	, nodes_(8)
	, connections_(128)
	, connections_mutex_()
{}

void ForwardModule::Initiate()
{
	// create mapping for nodes
	std::for_each(std::begin(app_.resource_manager().AllResources()), std::end(app_.resource_manager().AllResources())
		, [this](const std::unique_ptr<layinfo::AppResource>& in)
	{
		if( in->type == "sys_relay" && in->dev_tag == utility::DevTag() )
		{
			if(!this->nodes_.insert( std::make_pair(in->node, in->pid) ).second)
			{
				ThrowTException( CoreErrorCategory::ErrorCode::BAD_ARGUMENT
					, "ForwardModule::Initiate"
					, "More than one sys_relay for node: " + in->node );
			}
		}
	});
}

void ForwardModule::AddConnection(unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	std::lock_guard<std::mutex> lock(connections_mutex_);
	connections_[pid] = pconn;
}

bool ForwardModule::CheckForward(const TSystem::Message& msg)
{
	// forward msg to the pid
	static auto forward_msg = [this](unsigned short pid, const TSystem::Message& msg)->bool
	{
		std::lock_guard<std::mutex> lock(connections_mutex_);
		auto iter = connections_.find(pid);
		if( iter != connections_.end() )
		{
			if( iter->second->status() == communication::Connection::Status::connected )
				iter->second->AsyncSend( ForwardEncode(msg) );
			else
			{
				LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_NETWORK
					, "ForwardModule::CheckForward"
					, utility::FormatStr("Forwarding fail due to disocnnected resource: %d", pid)
					, app_.local_logger() );
				// TODO how to handle disconnected connections
				connections_.erase(iter);
			}
			return true;
		}else
			return false;
	};

	unsigned short receiver_pid = msg.ReceiverPid();
	if( receiver_pid && receiver_pid != app_.pid() )
	{
		if( !forward_msg(receiver_pid, msg) ) // if not pid of this node
		{
			// find node pid of the pid
			const layinfo::AppResource& res = app_.resource_manager().FindUnique( receiver_pid );
			if( !res )
			{
				LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT
					, "ForwardModule::CheckForward"
					, utility::FormatStr("Forwarding fail due to out of node resource: %d", receiver_pid)
					, app_.local_logger() );
			}else
			{
				// find sys_relay for node
				auto iter_node = nodes_.find( res.node );
				if( iter_node != nodes_.end() )
				{
					forward_msg(iter_node->second, msg);
				}else
				{
					LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT
						, "ForwardModule::CheckForward"
						, "Forwarding fail due connected node relay: " + res.node
						, app_.local_logger() );
				}
			}
		}
		return true;
	}

	return false;
}
