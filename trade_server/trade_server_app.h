#ifndef  TRADE_SERVER_APP_H_SDF32SDFSD_
#define TRADE_SERVER_APP_H_SDF32SDFSD_

#include <TLib/tool/tsystem_server_appbase.h>

#include "stock_ticker.h"


using namespace TSystem;

class TradeServerApp : public ServerAppBase
{
public:
    TradeServerApp(const std::string &name, const std::string &version);
    ~TradeServerApp(){}

    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override {};
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te) override {};

    virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;
	virtual void UpdateState() override;

    void Initiate();

private:

	TaskStrand  tick_strand_;
	//TaskStrand  index_tick_strand_;

	bool exit_flag_;

	bool ticker_enable_flag_;

	std::shared_ptr<StockTicker>  stock_ticker_;
	//std::shared_ptr<IndexTicker>  index_ticker_;
	int stock_ticker_life_count_;
};

#endif // TRADE_SERVER_APP_H_SDF32SDFSD_