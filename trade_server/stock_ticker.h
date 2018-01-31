#ifndef STOCK_TICKER_H_3SDFS_
#define STOCK_TICKER_H_3SDFS_

#include <mutex>
#include <memory>
#include <unordered_map>
 

#include <TLib/core/tsystem_local_logger.h>

#include "sys_common.h" 

namespace TSystem
{
    class TaskStrand;
}

class StrategyTask; 

// <task id, task>
typedef std::unordered_map<unsigned int, std::shared_ptr<StrategyTask> > TTaskIdMapStrategyTask;
// <stock code, task_id>
typedef std::unordered_map<std::string, std::list<unsigned int> > TCodeMapTasks;

static const unsigned int cst_result_len = 1024 * 1024;
static const unsigned int cst_error_len = 1024;
static const unsigned int cst_max_stock_code_count = 400;

typedef std::tuple<std::string, std::shared_ptr<QuotesData> > T_codeQuoteDateTuple;

class  Handler
{
public:
	virtual void Procedure() = 0;
};

class TradeServerApp;
class StockTicker : public Handler
{
public:

    StockTicker(TradeServerApp *app, TSystem::LocalLogger  &logger, TypeMarket market
		, std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > &&p_list, const std::string &tag);
    virtual ~StockTicker();

    virtual void Procedure() override;

    virtual bool Init();

    void Run();

    void Register(const std::shared_ptr<StrategyTask> & task);
    void UnRegister(unsigned int task_id);
    void ClearAllTask();

    bool GetSecurityBars(int Category, int Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);

    bool GetQuotes(char* stock_codes[], short count, Buffer &Result);

    void DecodeStkQuoteResult(Buffer &Result, std::list<T_codeQuoteDateTuple> *ret_quotes_data=nullptr
            , std::function<void(const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data)> tell_all_rel_task=nullptr);

    void TellAllRelTasks(const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data);

protected:
      
	static bool SetUpHqApi();
	static bool has_hq_api_setup_;
	std::mutex  hq_api_connect_mutex_;
	static bool has_hq_api_connected_;

    TradeServerApp  *app_;
    TSystem::TaskStrand  strand_; 
	TypeMarket market_;
    std::vector<std::shared_ptr<T_CodeBrokerTaskTables> >  code_broker_task_tbllist_;
	std::string tag_;

    TTaskIdMapStrategyTask  registered_tasks_;
     
    TCodeMapTasks  codes_taskids_;

	std::mutex  tasks_list_mutex_;
	
    TSystem::LocalLogger  &logger_;

	int stock_ticker_life_count_;
};

#if 0 
//////////////////////////////////////////////////////////////////
// IndexTicker
//////////////////////////////////////////////////////////////////

class IndexTicker : public StockTicker
{
public:

    IndexTicker(TSystem::LocalLogger  &logger);
    ~IndexTicker();

    virtual void Procedure() override; 

    virtual bool Init() override;
      
	//StkQuoteGetQuoteDelegate StkQuoteGetQuote_;

private:
	
	static const unsigned char max_index_count = 6;
	HMODULE stk_quoter_moudle_;

	Buffer index_codes[max_index_count][1023];
};
#endif

#endif //STOCK_TICKER_H_3SDFS_