
#include "trade_server_app.h"
 

TradeServerApp::TradeServerApp() : ServerAppBase("server", "trade_server", "0.1")
{

}

void TradeServerApp::Initiate()
{
    option_load_class_id(false);
    //LoadClassID(msg_system_);
    //option_load_config(true);
    ServerAppBase::Initiate();

    StartPort();
}

void TradeServerApp::HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg)
{

}

void TradeServerApp::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) 
{

}

void TradeServerApp::UpdateState()
{

}
