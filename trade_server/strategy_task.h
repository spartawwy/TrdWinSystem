#ifndef  STRATEGY_TASK_H_SDFDS_
#define STRATEGY_TASK_H_SDFDS_

#include <TLib/core/tsystem_time.h>
#include <string>
 
#include <list>
#include <chrono>
 

#include <TLib/core/tsystem_task_service.h>

#include "mutex_wrapper.h"
#include "sys_common.h"


class TradeServerApp;
class StrategyTask
{
public:
    
    StrategyTask(T_TaskInformation &task_info, TradeServerApp *app);

    virtual ~StrategyTask()
    { 
        //timed_mutex_.unlock(); 
    }

    virtual void HandleQuoteData() = 0;
	virtual std::string Detail(){ return "";}
    virtual void UnReg(){ }

    bool IsPriceJumpUp(double pre_price, double cur_price);
    bool IsPriceJumpDown(double pre_price, double cur_price);
     
    TradeServerApp * app() { return app_;}

    unsigned int  task_id() { return para_.id; }
    const char* stock_code() { return para_.stock.c_str(); }
    char* code_data() { return const_cast<char*>(para_.stock.c_str()); }
    T_TaskInformation& task_info() { return para_; }

    double  cur_price() { return cur_price_; }

    TypeMarket  market_type() const { return market_type_; }

    void SetOriginalState(TaskCurrentState val) { para_.state = static_cast<int>(val); }
    bool is_to_run() const 
    { 
        return para_.state != static_cast<int>(TaskCurrentState::STOP);
    }

    double GetQuoteTargetPrice(const QuotesData& data, bool is_buy);

    void ObtainData(std::shared_ptr<QuotesData> &data);
    
    void cur_state(TaskCurrentState val) { cur_state_ = val; }
    TaskCurrentState cur_state() { return cur_state_; }

    TSystem::TimePoint tp_start() { return tp_start_; }
    TSystem::TimePoint tp_end() { return tp_end_; }

    unsigned int life_count_;
    
protected:
     
   int HandleSellByStockPosition(double price, bool remove_task = true);
   int GetTototalPosition();

   TradeServerApp  *app_;
   T_TaskInformation  para_;
    
   TypeMarket  market_type_;
   
   std::list<std::shared_ptr<QuotesData> > quote_data_queue_;
    
   TSystem::TimePoint  tp_start_;
   TSystem::TimePoint  tp_end_;

   double  cur_price_;
   
   volatile TaskCurrentState cur_state_;
   bool is_waitting_removed_; 

   TSystem::TaskStrand   strand_;
     
   TimedMutexWrapper  timed_mutex_wrapper_;
};

#endif