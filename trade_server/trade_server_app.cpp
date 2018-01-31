#include "trade_server_app.h"
 
#include <TLib/core/tsystem_utility_functions.h>
#include "WINNERLib/winner_message_system.h"

#include "sys_common.h"
 

static const unsigned int cst_normal_timer_interval = 2000;
static const unsigned int cst_max_stocks_a_query = 80;
static const unsigned int cst_max_querys_a_time = 5;

TradeServerApp::TradeServerApp(const std::string &name, const std::string &version) : ServerAppBase("trade_server", name, version)
	/*, tick_strand_(task_pool())
	, tick_strand_1_(task_pool())*/
	, exit_flag_(false)
	, ticker_enable_flag_(true)
	/*, stock_ticker_life_count_(0)*/
	, db_moudle_(this)
	, id_brokers_(100)
	, id_users_(1024)
	, id_accounts_(1024*2)
	/*, code_table_container_beg_0_(std::make_shared<T_CodeMapTableList>(1000)) 
	, code_table_container_beg_3_(std::make_shared<T_CodeMapTableList>(1000)) 
	, code_table_container_beg_6_(std::make_shared<T_CodeMapTableList>(1800))  */
{ 
    code_table_container_beg_0_.reserve(1000);
    code_table_container_beg_3_.reserve(1000);
    code_table_container_beg_6_.reserve(1800);
}

void TradeServerApp::Initiate()
{
    option_load_class_id(false);
    WINNERSystem::LoadClassID(msg_system_);
    //option_load_config(true);
    ServerAppBase::Initiate();

	db_moudle_.Init();

	// todo: create tick_strand_list4stocks_x_ base on code_table_container_beg_x_
     
    const unsigned int max_strand_stocks = cst_max_stocks_a_query * cst_max_querys_a_time;

    unsigned int num = code_table_container_beg_0_.size() / max_strand_stocks;
    for( unsigned int i = 0; i < num; ++i )
    {  
		decltype(code_table_container_beg_0_) container;

		for( unsigned int j = i*max_strand_stocks; j < (i+1)*max_strand_stocks; ++j )
		{
			container.push_back(code_table_container_beg_0_.at(j));
		} 
		auto ticker = std::make_shared<StockTicker>(this, this->local_logger(), TypeMarket::SZ
			, std::move(container), utility::FormatStr("0_begin_%u", i));
		tick_strand_list4stocks_0_.emplace_back(std::move(ticker));
    }

    if( code_table_container_beg_0_.size() % max_strand_stocks > 0 )
    {
		decltype(code_table_container_beg_0_) container;
		for( unsigned int j = num*max_strand_stocks; j < num*max_strand_stocks + code_table_container_beg_0_.size() % max_strand_stocks; ++j )
		{
			container.push_back(code_table_container_beg_0_.at(j));
		} 
		auto ticker = std::make_shared<StockTicker>(this, this->local_logger(), TypeMarket::SZ
			, std::move(container), utility::FormatStr("0_begin_%u", num));
		tick_strand_list4stocks_0_.emplace_back(std::move(ticker));
    }
     
	for( auto item : tick_strand_list4stocks_0_)
	{
		item->Init();
		item->Run();
	}
    // todo: for code_table_container_beg_3_  code_table_container_beg_6_ 
    
	  
    //stock_ticker_ = std::make_shared<StockTicker>(this->local_logger());
    //stock_ticker_->Init();

	//-----------ticker main loop----------
#if 0
	task_pool().PostTask([this]()
	{
		while(!this->exit_flag_)
		{
			Delay(cst_ticker_update_interval);

			if( !this->ticker_enable_flag_ )
				continue;

			tick_strand_.PostTask([this]()
			{
				this->stock_ticker_->Procedure();
				this->stock_ticker_life_count_ = 0;
			});
 
		}
		//qDebug() << "out loop \n";
	});
#endif
	//----------------

    StartPort();
}

std::shared_ptr<TaskStrand> TradeServerApp::CreateStrand()
{
    return std::make_shared<TaskStrand>(task_pool());
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
