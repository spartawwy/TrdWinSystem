#include "trade_server_app.h"
 
#include "sys_common.h"

#include "WINNERLib/winner_message_system.h"

static const int cst_ticker_update_interval = 2000;  //ms :notice value have to be bigger than 1000
static const int cst_normal_timer_interval = 2000;

TradeServerApp::TradeServerApp(const std::string &name, const std::string &version) : ServerAppBase("trade_server", name, version)
	, tick_strand_(task_pool())
	, exit_flag_(false)
	, ticker_enable_flag_(true)
	, stock_ticker_life_count_(0)
{

}

void TradeServerApp::Initiate()
{
    option_load_class_id(false);
    WINNERSystem::LoadClassID(msg_system_);
    //option_load_config(true);
    ServerAppBase::Initiate();

	//-----------ticker main loop----------
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
#if 0
			index_tick_strand_.PostTask([this]()
			{
				this->index_ticker_->Procedure();
				this->index_ticker_life_count_ = 0;
			});
#endif
		}
		//qDebug() << "out loop \n";
	});
	//----------------

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
