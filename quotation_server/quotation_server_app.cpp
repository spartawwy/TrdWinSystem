#include "quotation_server_app.h"

#include <iostream>

#include "Python.h"

#include <TLib/tool/tsystem_connection_handshake.pb.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_return_code.h>

#include "WINNERLib/winner_message_system.h"


//#pragma comment(lib, "python36.lib")

QuotationServerApp::QuotationServerApp(const std::string &name, const std::string &version)
	: ServerAppBase("quotation_server", name, version)
{
	
}

QuotationServerApp::~QuotationServerApp()
{
	Py_Finalize();
}

void QuotationServerApp::Initiate()
{
	option_load_class_id(false);
	WINNERSystem::LoadClassID(msg_system_);
	//option_load_config(true);
	ServerAppBase::Initiate();

	InitPython();

	/*db_moudle_.Init();

	CreateStockTickers();*/
	SetupServerSocket();
	 
	StartPort();
}

void QuotationServerApp::InitPython()
{
	int r = Py_IsInitialized();  //1为已经初始化了
	if (r != 0)
	{
		std::cout << "init python fail " << std::endl;
		// todo: throw exception
		throw ""; 
	}

	Py_Initialize();  
	auto p_main_Module = PyImport_ImportModule("__main__");
	if (!p_main_Module)
	{
		throw "";
	}
	auto pDict = PyModule_GetDict(p_main_Module);
	if ( !pDict ) 
	{          
		throw ""; 
	}     

	PyRun_SimpleString("import sys");
}

void QuotationServerApp::HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg)
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
			//this->forward_module_.AddConnection(hs.pid(), pconn);

			// add connection for master selection module
			//this->master_selection_module_.AsyncAddResource(res.raid_id, res.pid, pconn);
		}
	}
}

void QuotationServerApp::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) 
{

}

void QuotationServerApp::UpdateState()
{

}

void QuotationServerApp::SetupServerSocket()
{
	msg_handlers_.RegisterHandler("UserRequest", [this](communication::Connection* p, const Message& msg)
	{
		auto req = std::make_shared<UserRequest>();
		if( Decode(msg, *req, this->msg_system()) )
			this->task_pool_.PostTask( std::bind(&QuotationServerApp::HandleUserRequest, this, std::move(req), p->shared_this()));
	});

    msg_handlers_.RegisterHandler("QuotationRequest", [this](communication::Connection* p, const Message& msg)
    {
        auto req = std::make_shared<QuotationRequest>();
        if( Decode(msg, *req, this->msg_system()) )
            this->task_pool_.PostTask( std::bind(&QuotationServerApp::HandleQuotationRequest, this, std::move(req), p->shared_this()));
    });
}

void QuotationServerApp::HandleUserRequest(std::shared_ptr<UserRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
	switch (req->request_type())
	{
	case RequestType::LOGIN:
		HandleUserLogin(*req, pconn);
		break;
	case RequestType::LOGOUT:
		//HandleUserLogout(*req, pconn);
		break;
	default:
		break;
	}
}

void QuotationServerApp::HandleUserLogin(const UserRequest& req, const std::shared_ptr<communication::Connection>& pconn)
{
	SendRequestAck(req.user_id(), req.request_id(), req.request_type(), pconn);
}

void QuotationServerApp::HandleQuotationRequest(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
    QuotationMessage quotation_msg;
    quotation_msg.set_code(req->code());
    pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) );
}
void QuotationServerApp::SendRequestAck(int user_id, int req_id, RequestType type, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	UserRequestAck ack;
	ack.set_req_type(type);
	ack.set_user_id(user_id);
	TSystem::FillRequestAck(req_id, *ack.mutable_req_ack());
	pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
}