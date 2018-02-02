#include "trade_server_app.h"
 
#include <array>

#include <TLib/core/tsystem_utility_functions.h>
#include "WINNERLib/winner_message_system.h"

#include "sys_common.h"
 

static const unsigned int cst_normal_timer_interval = 2000;
static const unsigned int cst_max_stocks_a_query = 80;
static const unsigned int cst_max_querys_a_time = 5;

TradeServerApp::TradeServerApp(const std::string &name, const std::string &version) : ServerAppBase("trade_server", name, version)
	, exit_flag_(false)
	, ticker_enable_flag_(true)
	, db_moudle_(this)
	, id_brokers_(100)
	, id_users_(1024)
	, id_accounts_(1024*2) 
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
#if 1
    std::array< std::vector<std::shared_ptr<T_CodeBrokerTaskTables> >*, 3> 
        tables_arry = {&code_table_container_beg_0_, &code_table_container_beg_3_, &code_table_container_beg_6_ };
    //code_table_container_beg_0_[0]->stock_.substr(0, 1) ;
    std::array<std::list<std::shared_ptr<StockTicker> >*, 3> 
        tick_list_arry = { &tick_strand_list4stocks_0_, &tick_strand_list4stocks_3_, &tick_strand_list4stocks_6_ };
    //std::for_each( std::begin(tables_arry), std::end(tables_arry), [max_strand_stocks, this](std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > * entry)
    std::array<std::string, 3> tag_arry = {"0xxxxx_", "3xxxxx_", "6xxxxx_"};

    // 0xxxxx, 3xxxxx, 6xxxxx
    for( unsigned int index = 0; index < tables_arry.size(); ++index )
    { 
        std::string tag = tables_arry[index]->at(0)->stock_.substr(0, 1); 

        unsigned int num = tables_arry[index]->size() / max_strand_stocks;
        unsigned int i = 0;
         // a contianer( for 0xxxxx begin stocks) has many ticker, 
        for( ; i < num; ++i )
        {  
		    std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > stock_code_container;
            // distribute stock codes (segment) to this container 
		    for( unsigned int j = i*max_strand_stocks; j < (i+1)*max_strand_stocks; ++j )
		    {
			    stock_code_container.push_back(tables_arry[index]->at(j));
		    } 
            // related container to ticker
		    auto ticker = std::make_shared<StockTicker>(this, this->local_logger(), TypeMarket::SZ
			    , std::move(stock_code_container), utility::FormatStr("%sxxxxx_%u_", tag, i));

            // push ticker to this ticker list
		    tick_list_arry[index]->emplace_back(std::move(ticker));
        }

        // tail tickers 
        if( tables_arry[index]->size() % max_strand_stocks > 0 )
        {
		    std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > container;
		    for( unsigned int j = num*max_strand_stocks; j < num*max_strand_stocks + tables_arry[index]->size() % max_strand_stocks; ++j )
		    {
			    container.push_back(tables_arry[index]->at(j));
		    } 
		    auto ticker = std::make_shared<StockTicker>(this, this->local_logger(), TypeMarket::SZ
			    , std::move(container), utility::FormatStr("%sxxxxx_%u_", tag, i));
             // push ticker to this ticker list
		    tick_list_arry[index]->emplace_back(std::move(ticker));
        }
     
        for( auto item : *tick_list_arry[index])
	    {
		    item->Init();
		    item->Run();
	    }
    }
#endif

#if 0
    unsigned int num = code_table_container_beg_0_.size() / max_strand_stocks;
    // a contianer( for 0xxxxx begin stocks) has many ticker, 
    for( unsigned int i = 0; i < num; ++i )
    {  
		decltype(code_table_container_beg_0_) stock_code_container;

        // distribute stock codes (segment) to this container 
		for( unsigned int j = i*max_strand_stocks; j < (i+1)*max_strand_stocks; ++j )
		{
			stock_code_container.push_back(code_table_container_beg_0_.at(j));
		} 
        // related container to ticker
		auto ticker = std::make_shared<StockTicker>(this, this->local_logger(), TypeMarket::SZ
			, std::move(stock_code_container), utility::FormatStr("0xxxxx_%u_", i));
        // push ticker to this ticker list
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
			, std::move(container), utility::FormatStr("0xxxxx_%u_", num));
		tick_strand_list4stocks_0_.emplace_back(std::move(ticker));
    }
     
	for( auto item : tick_strand_list4stocks_0_)
	{
		item->Init();
		item->Run();
	}
#endif
    // todo: for code_table_container_beg_3_  code_table_container_beg_6_ 
    
	  
    //stock_ticker_ = std::make_shared<StockTicker>(this->local_logger());
    //stock_ticker_->Init();

	//-----------ticker main loop---------- 
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
