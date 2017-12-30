#include "intermface.h"

#include <iostream>

#include <Tlib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/tool/tsystem_connection_handshake.pb.h>

#include "WinnerLib/winner_message_system.h"

using namespace TSystem;

Intermface::Intermface(bool monitor_flag) : ServerClientAppBase("misc", "intermface", "0.1")
	, msg_handlers_(msg_system_)
	, pconn_(nullptr)
	, async_ops_()
	, wze_client_socket_(*this)
	, monitor_flag_(monitor_flag)
{ 
}

Intermface::~Intermface()
{ 
	FireShutdown();
}

void Intermface::Initiate()
{
	// load class id
	option_load_class_id(false);
	WINNERSystem::LoadClassID(msg_system_);

	// set option and initiate
	option_dir_type(AppBase::DirType::STAND_ALONE_APP);
	ServerClientAppBase::Initiate();

	//------------------
	// register handler
	//------------------
	msg_handlers_.RegisterHandler( "HandShake", [this](communication::Connection* pconn, const Message& msg)
	{
		HandShake hs;
		if( Decode(msg, hs, this->msg_system_) )
		{
			local_logger().LogLocal( utility::FormatStr("receive handshake from connection: %d", pconn->connid()));
			pconn->hand_shaked(true);

			//if( monitor_flag_ )
			//	SubscribeMarketData(nullptr); // subscribe all products' market data information 
		}
	});
}

void Intermface::Shutdown()
{
	ServerClientAppBase::Shutdown();
}

void Intermface::Start(const std::string& res_name)
{
	// connect
	const layinfo::AppResource target_res = resource_manager().FindUnique(res_name);

	TError te;
	pconn_ = ConnectResource(target_res, te);
	if( te )
		throw TException( std::move(te) );

	// link and start
	msg_handlers_.LinkConnection(pconn_);
	pconn_->Start();

	// send handshake
	local_logger().LogLocal( utility::FormatStr("sending handshake to resource: %s", res_name.c_str()) );
	SendHandShake(*this, pconn_);

	if( !monitor_flag_ ) // control mode
	{
		// init ecp client socket
		wze_client_socket_.Connection(pconn_);

		// process command
		if( !monitor_flag_ )
			task_pool_.DispatchTask( std::bind(&Intermface::ParseCmd, this) );
	}
}

void Intermface::ParseCmd()
{
	std::string cmd;
	std::vector<std::string> args;

	while( !HasShutdown() )
	{
		std::getline(std::cin, cmd);
		std::cout << cmd << std::endl;
	}
}

void Intermface::ProcessCmd(const std::vector<std::string>& cmd)
{

}

void Intermface::HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg)
{

}

void Intermface::HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te)
{

}