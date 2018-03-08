#include "intermface.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

#include <Tlib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_core_error.h>
#include <TLib/core/tsystem_time.h>

#include <TLib/tool/tsystem_connection_handshake.pb.h>

#include "WinnerLib/winner_message_system.h"
#include "WinnerLib/quotation_msg.pb.h"

using namespace TSystem;

Intermface::Intermface(bool monitor_flag) : ServerClientAppBase("misc", "intermface", "0.1")
	, msg_handlers_(msg_system_)
	, pconn_(nullptr)
	, async_ops_()
	, wzf_client_socket_(*this)
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

    msg_handlers_.RegisterHandler( "QuotationMessage", [this](communication::Connection* pconn, const Message& msg)
    {
        QuotationMessage quotation_msg;
        if( Decode(msg, quotation_msg, this->msg_system_) )
        {
            local_logger().LogLocal( utility::FormatStr("receive QuotationMessage from connection: %d", pconn->connid()));
            
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
		wzf_client_socket_.Connection(pconn_);

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
		args = TSystem::utility::split(cmd);

		if( args.size() )
		{
			if( args[0] == "EOF" )
			{
				Shutdown();
				break;
			}else
			{
				ProcessCmd(args);
			}
		}
	}
}

void Intermface::ProcessCmd(const std::vector<std::string>& cmd)
{
	static auto PrintErrorMsg = [](/*const std::string& error*/)
	{
		std::cout << "Command is not correct! " << std::endl;
	};
	try
	{
		if( cmd[0] == "Login" && cmd.size() > 2 ) // cmd formart: Login user_id password
		{
			unsigned int user_id = boost::lexical_cast<unsigned int>(cmd[1]);
			Login(user_id, cmd[2]);
		}else if( cmd[0] == "Logout" && cmd.size() > 1 ) // cmd formart: Logout user_id
		{
			unsigned int user_id = boost::lexical_cast<unsigned int>(cmd[1]);
			//Logout(user_id);
		}else if( cmd[0] == "QRequest" && cmd.size() > 1 ) // cmd formart: Request code type begin_time end_time
		{
            RequestQuotation(cmd[1]);
		}
	}catch(const boost::bad_lexical_cast& e)
	{
		PrintErrorMsg();
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_ARGUMENT
			, "InternalTerminal::ProcessCmd"
			, FormatThirdPartyError("boost", 0, e.what())
			, local_logger());
	}catch(const TException& e)
	{
		PrintErrorMsg();
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_ARGUMENT
			, "InternalTerminal::ProcessCmd"
			, ErrorString(e.error())
			, local_logger());
	}
}

void Intermface::HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg)
{

}

void Intermface::HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te)
{

}


void Intermface::Login(unsigned int user_id, const std::string& password)
{
	bool seq = false;
	TSystem::ScopedHandler a(async_ops_, [&seq](TError& ec) { if(!ec) seq = true; });
	wzf_client_socket_.SendUserRequest(a.SequenceID(), RequestType::LOGIN, user_id, password);

	if( TSystem::WaitFor( [&seq]()->bool{return seq;}, 15*1000) )
	{
		std::string log_str = utility::FormatStr("user %u Login", user_id);
		std::cout << log_str << std::endl;
		local_logger().LogLocal(std::move(log_str));
	}else
		ThrowTException(TSystem::CoreErrorCategory::ErrorCode::BAD_CONTENT, "InternalTerminal::Login", "bad login");
}

void Intermface::RequestQuotation(const std::string &code)
{
    wzf_client_socket_.SendQuotationRequst(code); 
    std::cout << "RequestQuotation " << code << std::endl;
}