#include "log_msg_module.h"
#include "winner_relay_app.h"

#include <TLib/core/tsystem_core_paths.h>
#include <TLib/core/tsystem_serialization.h>

using namespace TSystem;

LogMsgModule::LogMsgModule(WinnerRelayApp& app)
	: app_(app)
{}

void LogMsgModule::Initiate()
{
	// init topic
	static unsigned short log_msg_id = app_.msg_system().FindClassID("LogMessage");
	std::for_each( std::begin(app_.resource_manager().AllResources()), std::end(app_.resource_manager().AllResources())
		, [this](const std::unique_ptr<layinfo::AppResource>& in)
	{
		if( in->node == app_.node() && in->dev_tag == utility::DevTag() )
			app_.topic_server_.AddTopicData( log_msg_id, utility::FormatStr("%d", in->pid), app_.task_pool() );
	});

	// register handler
	app_.msg_handlers_.RegisterHandler("LogMessage", [this](communication::Connection* pcon, const Message& msg)
	{
		LogMessage log_msg;
		if( Decode(msg, log_msg, app_.msg_system()) )
		{
			communication::TopicData* p = app_.topic_server_.Find( log_msg_id, utility::FormatStr("%d", log_msg.pid()) );
			if( p )
				p->AsyncUpdateData( ForwardEncode(msg) );
		}
	});
}
