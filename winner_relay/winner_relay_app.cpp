#include "winner_relay_app.h"

#include <TLib/core/tsystem_serialization.h>
#include <TLib/tool/tsystem_connection_handshake.pb.h>

#include "WINNERLib/winner_message_system.h"

using namespace TSystem;

WinnerRelayApp::WinnerRelayApp(const std::string& name, const std::string& version)
	: ServerAppBase("sys_relay", name, version)
	, heartbeat_module_(*this)
	, master_selection_module_(*this)
	, forward_module_(*this)
	, log_msg_module_(*this)
   /* , info_msg_module_(*this)
    , raid_state_module_(*this)*/
{}

WinnerRelayApp::~WinnerRelayApp()
{
	FireShutdown();
}

void WinnerRelayApp::Initiate()
{
	// load wze class id
	option_load_class_id(false);
    WINNERSystem::LoadClassID(msg_system_);

	ServerAppBase::Initiate();

	// re-direct generic reqeust to generic_request_kernel_
	msg_handlers_.RegisterHandler("GenericRequest", [this](communication::Connection* pconn, const Message& msg)
	{  
		if( forward_module_.CheckForward(msg) )
			return;

		this->generic_request_kernel_.HandleMessage(pconn, msg);
	});

	// forward 
	msg_handlers_.RegisterHandler("RequestAck", [this](communication::Connection* pconn, const Message& msg)
	{  
		forward_module_.CheckForward(msg);
	});

	//-------------
	// modules
	//-------------

	// heartbeat module
	heartbeat_module_.Initiate();

	// forward module
	forward_module_.Initiate();

	// log msg module
	log_msg_module_.Initiate();

    // info msg module
    //info_msg_module_.Initiate();

    // raid state module 
    //raid_state_module_.Initiate();

	// start port
	StartPort();
}

void WinnerRelayApp::UpdateState()
{
	heartbeat_module_.Start();
	heart_beat_.UpdateStatus(Heartbeat::ON_LINE, pid());
}

void WinnerRelayApp::HandleInboundHandShake(TSystem::communication::Connection* p, const Message& msg)
{
	HandShake hs;

	if(Decode(msg, hs, msg_system_))
	{
		const layinfo::AppResource& res = resource_manager_.FindUnique(hs.pid()); 
		if( res )
		{
			auto pconn = p->shared_this();

			p->hand_shaked(true);
			SetupInboundConnection(pconn, res);

			// add connection for forward module
			this->forward_module_.AddConnection(hs.pid(), pconn);

			// add connection for master selection module
			this->master_selection_module_.AsyncAddResource(res.raid_id, res.pid, pconn);
		}
	}
}

void WinnerRelayApp::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te)
{
	master_selection_module_.AsyncRemoveResource(pconn->connid());
}

void WinnerRelayApp::Shutdown()
{
	heart_beat_.UpdateStatus(Heartbeat::OFF_LINE, pid());
	heartbeat_module_.Shutdown();
	ServerAppBase::Shutdown();
}