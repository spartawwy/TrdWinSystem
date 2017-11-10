#ifndef  TRADE_SERVER_APP_H_SDF32SDFSD_
#define TRADE_SERVER_APP_H_SDF32SDFSD_

#include <TLib/tool/tsystem_server_appbase.h>

using namespace TSystem;

class TradeServerApp : public ServerAppBase
{
public:
    TradeServerApp();
    ~TradeServerApp(){}

    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override {};
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te) override {};

    virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;
	virtual void UpdateState() override;

    void Initiate();

};

#endif // TRADE_SERVER_APP_H_SDF32SDFSD_