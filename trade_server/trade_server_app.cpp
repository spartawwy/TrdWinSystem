
#include "trade_server_app.h"
 
#include "WINNERLib/winner_message_system.h"

TradeServerApp::TradeServerApp(const std::string &name, const std::string &version) : ServerAppBase("trade_server", name, version)
{

}

void TradeServerApp::Initiate()
{
    option_load_class_id(false);
    WINNERSystem::LoadClassID(msg_system_);
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
