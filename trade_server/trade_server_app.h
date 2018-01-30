#ifndef  TRADE_SERVER_APP_H_SDF32SDFSD_
#define TRADE_SERVER_APP_H_SDF32SDFSD_

#include <TLib/tool/tsystem_server_appbase.h>

#include "stock_ticker.h"
#include "db_moudle.h"

using namespace TSystem;

typedef std::tuple<std::shared_ptr<TaskStrand>, int> T_TupleStrandStockNum;

class Accounter;
class TradeServerApp : public ServerAppBase
{
public:
    TradeServerApp(const std::string &name, const std::string &version);
    ~TradeServerApp(){}

    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override 
    {
    };
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te) override {};

    virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;
	virtual void UpdateState() override;

    void Initiate();

private:

	TaskStrand  tick_strand_;
	TaskStrand  tick_strand_1_;

	std::list<T_TupleStrandStockNum> tick_strand_list4stocks_0_;
	std::list<T_TupleStrandStockNum> tick_strand_list4stocks_3_;
	std::list<T_TupleStrandStockNum> tick_strand_list4stocks_6_;

	bool exit_flag_;

	bool ticker_enable_flag_;

	std::shared_ptr<StockTicker>  stock_ticker_;
	//std::shared_ptr<IndexTicker>  index_ticker_;
	int stock_ticker_life_count_;

	DBMoudle  db_moudle_;
	std::unordered_map<int, std::shared_ptr<T_BrokerInfo> > id_brokers_;
	std::unordered_map<std::string, std::shared_ptr<T_UserInfo> > id_users_;

	std::unordered_map<int, std::shared_ptr<T_AccountInfo> > id_accounts_;
	
	
	std::shared_ptr<T_CodeMapTableList> code_table_container_beg_0_; //sz begin 0
	std::shared_ptr<T_CodeMapTableList> code_table_container_beg_3_; //sz begin 3
	std::shared_ptr<T_CodeMapTableList> code_table_container_beg_6_; //sh


	friend class DBMoudle;
};

#endif // TRADE_SERVER_APP_H_SDF32SDFSD_