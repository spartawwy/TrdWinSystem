#include "quotation_server_app.h"

#include <iostream>

#include "Python.h"

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

}

void QuotationServerApp::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) 
{

}

void QuotationServerApp::UpdateState()
{

}