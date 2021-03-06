#ifndef  TRADE_SERVER_APP_H_SDF32SDFSD_
#define TRADE_SERVER_APP_H_SDF32SDFSD_

#include <TLib/tool/tsystem_server_appbase.h>

#include "stock_ticker.h"
#include "db_moudle.h"

using namespace TSystem;

// (strand, stockNum)
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

    std::shared_ptr<TaskStrand> CreateStrand();
	bool exit_flag() const { return exit_flag_; } 
	bool ticker_enable_flag() const { return ticker_enable_flag_; }

    std::mutex& hq_api_connect_mutex() { return hq_api_connect_mutex_;}

private:

	void CreateStockTickers();

	std::list<std::shared_ptr<StockTicker> > tick_strand_list4stocks_0_;
	std::list<std::shared_ptr<StockTicker> > tick_strand_list4stocks_3_;
	std::list<std::shared_ptr<StockTicker> > tick_strand_list4stocks_6_;

	volatile bool exit_flag_; 
	volatile bool ticker_enable_flag_;

	//std::shared_ptr<StockTicker>  stock_ticker_; 
	

	DBMoudle  db_moudle_;
	// (broker id, T_BrokerInfo)
	std::unordered_map<int, std::shared_ptr<T_BrokerInfo> > id_brokers_;
	// (broker type, BrokerTaskTableName)
	std::unordered_map<int, std::string> type_broker_task_tablename_;
	std::unordered_map<std::string, std::shared_ptr<T_UserInfo> > id_users_;

	std::unordered_map<int, std::shared_ptr<T_AccountInfo> > id_accounts_;
	
	
    std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > code_table_container_beg_0_; //sz begin 0
	std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > code_table_container_beg_3_; //sz begin 3
	std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > code_table_container_beg_6_; //sh
	
	// (broker_type, (account_id, (task_id, taskinfo) ) ) 
	std::unordered_map<int, std::shared_ptr<std::unordered_map<int, T_Id_TaskInfo_Map> > > broker_account_taskid_info_;

    std::mutex  hq_api_connect_mutex_;

	friend class DBMoudle;
};

#endif // TRADE_SERVER_APP_H_SDF32SDFSD_