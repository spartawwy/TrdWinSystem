
#include "stock_ticker.h"

//#include <iostream>
  
#include <iostream>
#include <chrono>
  
#include <boost/lexical_cast.hpp>
 
#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/tool/tsystem_rational_number.h>
//#include "common.h"
#include "strategy_task.h"
//#include "winner_app.h"
#include <windows.h>
#include "TdxHqApi.h"

#include "sys_common.h"
#include "trade_server_app.h"

 //��ȡapi����
static TdxHq_ConnectDelegate TdxHq_Connect = nullptr;
static TdxHq_DisconnectDelegate TdxHq_Disconnect = nullptr;
static TdxHq_GetSecurityBarsDelegate TdxHq_GetSecurityBars = nullptr;
static TdxHq_GetIndexBarsDelegate TdxHq_GetIndexBars = nullptr;
static TdxHq_GetMinuteTimeDataDelegate TdxHq_GetMinuteTimeData = nullptr;
static TdxHq_GetHistoryMinuteTimeDataDelegate TdxHq_GetHistoryMinuteTimeData = nullptr;
static TdxHq_GetTransactionDataDelegate TdxHq_GetTransactionData = nullptr;
static TdxHq_GetHistoryTransactionDataDelegate TdxHq_GetHistoryTransactionData = nullptr;
static TdxHq_GetSecurityQuotesDelegate TdxHq_GetSecurityQuotes = nullptr;
static TdxHq_GetCompanyInfoCategoryDelegate TdxHq_GetCompanyInfoCategory = nullptr;
static TdxHq_GetCompanyInfoContentDelegate TdxHq_GetCompanyInfoContent = nullptr;
static TdxHq_GetXDXRInfoDelegate TdxHq_GetXDXRInfo = nullptr;
static TdxHq_GetFinanceInfoDelegate TdxHq_GetFinanceInfo = nullptr;

using namespace TSystem;

static char cst_hq_server[] = "122.224.66.108";
static int  cst_hq_port = 7709;

static const int cst_ticker_update_interval = 2000;  //ms :notice value have to be bigger than 1000

bool StockTicker::has_hq_api_setup_ = false;
bool StockTicker::has_hq_api_connected_ = false;

static bool is_codes_in(char* stock_codes[cst_max_stock_code_count], const char *str);
static int GetRegisteredCodes(TTaskIdMapStrategyTask  &registered_tasks, char* stock_codes[cst_max_stock_code_count], byte markets[cst_max_stock_code_count]);

StockTicker::StockTicker(TradeServerApp *app, TSystem::LocalLogger  &logger, TypeMarket market
						 , std::vector<std::shared_ptr<T_CodeBrokerTaskTables> > &&list, const std::string &tag)
    : app_(app)
	//, strand_( std::move(app->CreateStrand()) )
	, strand_(app->task_pool())
	, market_(market)
    , code_broker_task_tbllist_(std::move(list))
	, tag_(tag)
    , registered_tasks_(cst_max_stock_code_count)
    , codes_taskids_(cst_max_stock_code_count)
    , tasks_list_mutex_()
    , logger_(logger)
	, stock_ticker_life_count_(0)
{

}

StockTicker::~StockTicker()
{
    /*if( TdxApiHMODULE )
        FreeLibrary(TdxApiHMODULE);*/
    if( TdxHq_Disconnect )
        TdxHq_Disconnect();
}

bool StockTicker::SetUpHqApi()
{
	// ndchk: may be a ticker object need a dependence tdxapi moudle
	if( !has_hq_api_setup_ )
	{
		has_hq_api_setup_ = true;
		HMODULE TdxApiHMODULE = LoadLibrary("TdxHqApi20991230.dll");
		if( TdxApiHMODULE == nullptr )
		{
			has_hq_api_setup_ = false;
			std::cout << "LoadLibrary(TdxHqApi20991230.dll fail" << std::endl;
			//throw excepton;
			return false;
		}
		
		//��ȡapi����
		TdxHq_Connect = (TdxHq_ConnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Connect");
		TdxHq_Disconnect = (TdxHq_DisconnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Disconnect");
		TdxHq_GetSecurityBars = (TdxHq_GetSecurityBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityBars");
		TdxHq_GetIndexBars = (TdxHq_GetIndexBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetIndexBars");
		TdxHq_GetMinuteTimeData = (TdxHq_GetMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetMinuteTimeData");
		TdxHq_GetHistoryMinuteTimeData = (TdxHq_GetHistoryMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryMinuteTimeData");
		TdxHq_GetTransactionData = (TdxHq_GetTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetTransactionData");
		TdxHq_GetHistoryTransactionData = (TdxHq_GetHistoryTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryTransactionData");
		TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityQuotes");
		TdxHq_GetCompanyInfoCategory = (TdxHq_GetCompanyInfoCategoryDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoCategory");
		TdxHq_GetCompanyInfoContent = (TdxHq_GetCompanyInfoContentDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoContent");
		TdxHq_GetXDXRInfo = (TdxHq_GetXDXRInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetXDXRInfo");
		TdxHq_GetFinanceInfo = (TdxHq_GetFinanceInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetFinanceInfo");
		
	}
	return true;
}

bool StockTicker::Init()
{
    //����dll, dllҪ���Ƶ�debug��releaseĿ¼��,������ö��ֽ��ַ����������,�û����ʱ���Լ����Ƹ�������ʾ��С��λ���뾫��
    if( !SetUpHqApi() )
		return false;
	if( has_hq_api_connected_ )
		return true;
	else
		std::lock_guard<std::mutex> locker(app_->hq_api_connect_mutex());
	if( has_hq_api_connected_ )
		return true;
    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);
    
    bool bool1 = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
    if (!bool1)
    { 
        std::cout << ErrInfo.data() << std::endl;//����ʧ��
        logger_.LogLocal(std::string("StockTicker::Init TdxHq_Connect fail:") + Result.data());
        return false;
    }
    std::cout << Result.data() << std::endl;
    logger_.LogLocal(std::string("StockTicker::Init") + Result.data());
	has_hq_api_connected_ = true;
    return true;
}

void StockTicker::Run()
{
	assert(has_hq_api_connected_);
    // use task_pool and  strand to run Procedure
	app_->task_pool().PostTask([&, this]()
	{
		while( !this->app_->exit_flag() )
		{
			Delay(cst_ticker_update_interval);

			if( !this->app_->ticker_enable_flag() )
				continue;

			strand_.PostTask([this]()
			{
				this->Procedure();
				this->stock_ticker_life_count_ = 0;
			});

		}
	});
}

bool StockTicker::GetQuotes(char* stock_codes[], short count, Buffer &Result)
{
    byte markets[cst_max_stock_code_count];
    for( int i = 0; i < count; ++i )
    {
        markets[i] = (byte)GetStockMarketType(stock_codes[i]);
    }

    Buffer ErrInfo(cst_error_len);
    auto ret = TdxHq_GetSecurityQuotes(markets, stock_codes, count, Result.data(), ErrInfo.data());
    if ( !ret )
    {
        // reconnect -----------
        {
        std::lock_guard<std::mutex> locker(app_->hq_api_connect_mutex());
        if( !has_hq_api_connected_ )
        {  
            std::cout << ErrInfo.data() << std::endl;
            logger_.LogLocal(std::string("StockTicker::GetQuotes TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
            if( !strstr(ErrInfo.data(), "����������") )
                TdxHq_Disconnect();

            logger_.LogLocal(utility::FormatStr("StockTicker::GetQuotes TdxHq_Connect %s : %d ", cst_hq_server, cst_hq_port) );
            Result.reset(); 
            ErrInfo.reset();
            try
            {
            ret = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
            if ( !ret )
            {
                logger_.LogLocal(std::string("StockTicker::GetQuotes retry TdxHq_Connect fail:") + ErrInfo.data());
                return false;
            }
            logger_.LogLocal(utility::FormatStr("StockTicker::GetQuotes TdxHq_Connect ok!"));
            has_hq_api_connected_ = true;
            }catch(std::exception &e)
            {
                logger_.LogLocal(utility::FormatStr("StockTicker::GetQuotes exception:%s", e.what()));
                return false;
            }catch(...)
            {
                logger_.LogLocal("StockTicker::GetQuotes exception");
                return false;
            }
        }
        }
        // end ----------------

        Result.reset(); 
        ErrInfo.reset();
        // debug 
        for( int i = 0; i < count; ++i )
            logger_.LogLocal(utility::FormatStr("StockTicker::GetQuotes stock_codes[%d]:%s", i, stock_codes[i]));
        // end debug 
        ret = TdxHq_GetSecurityQuotes(markets, stock_codes, count, Result.data(), ErrInfo.data());
        if ( !ret )
        {
            logger_.LogLocal(std::string("StockTicker::GetQuotes retry TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
            return false;
        }
         
    }
    return true;
}

void StockTicker::DecodeStkQuoteResult(Buffer &Result, std::list<T_codeQuoteDateTuple> * ret_quotes_data
                                       , std::function<void (const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data)> tell_all_rel_task)
{
    auto tp_now = std::chrono::system_clock::now();
    time_t t_t = std::chrono::system_clock::to_time_t(tp_now); 

    std::string res_str = Result.data();
    TSystem::utility::replace_all_distinct(res_str, "\n", "\t");
     
    auto result_array = TSystem::utility::split(res_str, "\t");
    
    for( unsigned n = 1; n < result_array.size() / 44; ++n )
    {
        auto stock_code = result_array.at(1 + n * 44);

        auto task_ids_iter = codes_taskids_.find(stock_code);
        if( tell_all_rel_task )
        { 
            if( task_ids_iter == codes_taskids_.end() )
                continue;
        }
        auto quote_data = std::make_shared<QuotesData>();

        try
        {
        quote_data->cur_price = boost::lexical_cast<double>( result_array.at(3 + n * 44) );
        quote_data->active_degree = boost::lexical_cast<double>(result_array.at(42 + n * 44));

        quote_data->time_stamp = (__int64)t_t;

        quote_data->price_b_1 = boost::lexical_cast<double>(result_array.at(17 + n * 44));
        quote_data->price_s_1 = boost::lexical_cast<double>(result_array.at(18 + n * 44));
        quote_data->price_b_2 = boost::lexical_cast<double>(result_array.at(21 + n * 44));
        quote_data->price_s_2 = boost::lexical_cast<double>(result_array.at(22 + n * 44));
        quote_data->price_b_3 = boost::lexical_cast<double>(result_array.at(25 + n * 44));
        quote_data->price_s_3 = boost::lexical_cast<double>(result_array.at(26 + n * 44));
        quote_data->price_b_4 = boost::lexical_cast<double>(result_array.at(29 + n * 44));
        quote_data->price_s_4 = boost::lexical_cast<double>(result_array.at(30 + n * 44));
        quote_data->price_b_5 = boost::lexical_cast<double>(result_array.at(33 + n * 44));
        quote_data->price_s_5 = boost::lexical_cast<double>(result_array.at(34 + n * 44));
        }catch(const boost::exception&)
        {
            continue;
        }
        
        if( tell_all_rel_task )
            tell_all_rel_task(task_ids_iter->second, quote_data);
        if( ret_quotes_data )
            ret_quotes_data->push_back(std::make_tuple(stock_code, std::move(quote_data)));
    }
}

void StockTicker::Procedure()
{  
	const int re_count = 5;
	Buffer Result_0(cst_result_len);
	Buffer Result_1(cst_result_len);
	Buffer Result_2(cst_result_len);
	Buffer Result_3(cst_result_len);
	Buffer Result_4(cst_result_len);

	Buffer * result_array[re_count];
	result_array[0] = &Result_0;
	result_array[1] = &Result_1;
	result_array[2] = &Result_2;
	result_array[3] = &Result_3;
	result_array[4] = &Result_4;
	//printf("StockTicker::Procedure \n")
	const std::string file_tag = "ticker_" + tag_;
	//logger_.LogLocal( file_tag, "StockTicker::Procedure ");
	//return; //temp code

	/*for (int i = 0; i < re_count; ++i )
	{

	}*/
    Buffer ErrInfo(cst_error_len);

    //char* stock_codes[cst_max_stock_code_count];
    //memset(stock_codes, 0, sizeof(char*)*cst_max_stock_code_count);
      
    //byte markets[cst_max_stock_code_count];

    short stock_count = 0;
    
//#if 0
//    //--------------------------- 
//    stock_count = GetRegisteredCodes(registered_tasks_, stock_codes, markets);
//    if( stock_count < 1 )
//        return;
//#else
	Buffer stocks[cst_max_stock_code_count];
	
	char * stock_array[cst_max_stock_code_count];
	memset(stock_array, 0, sizeof(char*) * cst_max_stock_code_count);
	//int stk_count = GetBaseStocks(stock_array, MAX_STOCKS);
	int stk_count = 0;
	std::for_each( code_broker_task_tbllist_.begin(), code_broker_task_tbllist_.end(), [&stocks, &stock_array, &stk_count](std::shared_ptr<T_CodeBrokerTaskTables>& entry)
	{
		stock_array[stk_count] = stocks[stk_count].data();
		strcpy(stock_array[stk_count], entry->stock_.c_str());
		++stk_count;
	});
//#endif
	const int max_per_stock = 80;
	int i = 0;
	for (; i < stk_count / max_per_stock; ++i )
	{ 
		stock_count = 80;
		if( !GetQuotes(&stock_array[i * stock_count], stock_count, *result_array[i]) )
			return;
		// std::cout << result_array[i]->c_data()[0] << result_array[i]->c_data()[1] << result_array[i]->c_data()[2]  
		// << result_array[i]->c_data()[3] << result_array[i]->c_data()[4] << result_array[i]->c_data()[5] << std::endl;

		///std::cout << result_array[i]->c_data() << std::endl;
		///std::cout << "StockTicker::Procedure GetQuotes for " << i << " OK ret " << stock_count << " stocks " << std::endl;
		logger_.LogLocal(file_tag, result_array[i]->c_data());
		///DecodeStkQuoteResult(*result_array[i], nullptr, std::bind(&StockTicker::TellAllRelTasks, this, std::placeholders::_1, std::placeholders::_2));
	}
	if( stk_count % max_per_stock > 0)
	{
		if( !GetQuotes(&stock_array[i * stock_count], stk_count % max_per_stock, *result_array[i]) )
			return;
		logger_.LogLocal(file_tag, result_array[i]->c_data());
	}
    /*auto tp_now = std::chrono::system_clock::now();
    time_t t_t = std::chrono::system_clock::to_time_t(tp_now); */
    //--------------------------Docode result -------------

     
}
 
void StockTicker::TellAllRelTasks(const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data)
{
#if 0
    // throu id list which this stock code related   
    //std::lock_guard<std::mutex>  locker(tasks_list_mutex_);
    std::for_each( std::begin(id_list), std::end(id_list), [&, this](unsigned int id)
    {
        TTaskIdMapStrategyTask::iterator  task_iter = this->registered_tasks_.find(id);
        if( task_iter != this->registered_tasks_.end() )
        { 
            //qDebug() << std::get<1>(CurrentDateTime()).c_str() << " " << task_iter->second->stock_code() << " cur_pric:" << task_iter->second->cur_price() << "\n";
            task_iter->second->ObtainData(data);

            task_iter->second->life_count_ = 0;
			if( task_iter->second->cur_state() != TaskCurrentState::RUNNING )
			{
				task_iter->second->cur_state(TaskCurrentState::RUNNING);
				//task_iter->second->app()->Emit(task_iter->second.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
			}

        }else
        {
            // log error
            logger_.LogLocal(utility::FormatStr("error StockTicker::Procedure can't find task %d", id));
        }
    });
#endif
};
// insert into registered_tasks_
void StockTicker::Register(const std::shared_ptr<StrategyTask> & task)
{ //std::lock_guard<std::mutex>  locker(tasks_list_mutex_);

    if( registered_tasks_.find(task->task_id()) != registered_tasks_.end() )
        return;

    registered_tasks_.insert(std::make_pair(task->task_id(), task));

    auto iter = codes_taskids_.find(task->stock_code());
    if( iter == codes_taskids_.end() )
    {
        std::list<unsigned int> task_id_list;
        iter = codes_taskids_.insert( std::make_pair(task->stock_code(), std::move(task_id_list)) ).first;
    }

    iter->second.push_back(task->task_id());
}

void StockTicker::UnRegister(unsigned int task_id)
{
    std::lock_guard<std::mutex>  locker(tasks_list_mutex_);
    auto iter = registered_tasks_.find(task_id);
    if( iter == registered_tasks_.end() )
    {
        // logger error
        logger_.LogLocal(utility::FormatStr("warning StockTicker::UnRegister can't find task %d", task_id));
        return;
    } 

    // remove task id in codes_taskids_ inner list ------------

    auto code_task_iter = codes_taskids_.find(iter->second->code_data());
    assert( code_task_iter != codes_taskids_.end() );
    std::list<unsigned int>& id_list = code_task_iter->second;

    auto list_iter = id_list.begin();
    for( ; list_iter != id_list.end(); )
    {
        if( *list_iter == task_id )
        {
            id_list.erase(list_iter++);
            std::cout << " StockTicker::UnRegister " << task_id << " \n";
            logger_.LogLocal(utility::FormatStr("StockTicker::UnRegister %d", task_id));
            break;
        }else
            ++list_iter;
    }
    iter->second->UnReg();
	registered_tasks_.erase(iter);
}

void StockTicker::ClearAllTask()
{
    std::lock_guard<std::mutex>  locker(tasks_list_mutex_);
    std::for_each( std::begin(registered_tasks_), std::end(registered_tasks_), [this](TTaskIdMapStrategyTask::reference entry)
    {
        entry.second->cur_state(TaskCurrentState::STOP);
        //entry.second->app()->Emit(entry.second.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
    });
    registered_tasks_.clear();
    codes_taskids_.clear();
}

bool StockTicker::GetSecurityBars(int Category, int Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo)
{
    return TdxHq_GetSecurityBars(Category, Market, Zqdm, Start, Count, Result, ErrInfo);
}



#if 0 
//////////////////////////////////////////////////////////////////
// IndexTicker  for  index ticker
//////////////////////////////////////////////////////////////////

IndexTicker::IndexTicker(TSystem::LocalLogger  &logger)
    : StockTicker(logger)
	, stk_quoter_moudle_(nullptr)
	, StkQuoteGetQuote_(nullptr)
{

}

IndexTicker::~IndexTicker()
{ 
	if( stk_quoter_moudle_ ) 
		FreeLibrary(stk_quoter_moudle_);
	stk_quoter_moudle_ = nullptr;
}

bool IndexTicker::Init()
{
    //-----------------------StkQuoter-------------
	//SetCurrentEnvPath();
	char chBuf[1024] = {0};
	if( !::GetModuleFileName(NULL, chBuf, MAX_PATH) )  
		return false;   
	std::string strAppPath(chBuf);  

	auto nPos = strAppPath.rfind('\\');
	if( nPos > 0 )
	{   
		strAppPath = strAppPath.substr(0, nPos+1);  
	}  
	std::string stk_quote_full_path = strAppPath + "StkQuoter.dll";
	stk_quoter_moudle_ = LoadLibrary(stk_quote_full_path.c_str());
	if( !stk_quoter_moudle_ )
	{
		auto erro = GetLastError();
		QString info_str = QString("load %1 fail! error:%2").arg(stk_quote_full_path.c_str()).arg(erro);
		QMessageBox::information(nullptr, "info", info_str);
		//throw excepton;
		return false;
	}
	StkQuoteGetQuote_ = (StkQuoteGetQuoteDelegate)GetProcAddress(stk_quoter_moudle_, "StkQuoteGetQuote");
	assert(StkQuoteGetQuote_);
    return true;
}

// run in index_tick_strand_ 
void IndexTicker::Procedure()
{ 
	static auto are_codes_in = [](char codes[max_index_count][16], const char *str)
	{ 
		for(int i = 0; i < max_index_count; ++i)
		{
			if( codes[i] == nullptr )
				return false;
			if( !strcmp(codes[i], str) )
				return true;
		}
		return false;
	};
    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);

    char index_codes[max_index_count][16];
    memset(index_codes, 0, max_index_count*16);
       
    auto  cur_time = QTime::currentTime();
       
    //------------------find  task's stock code ,which state and time is fit---------
    int index_count = 0; 
    std::for_each( std::begin(registered_tasks_), std::end(registered_tasks_), [&](TTaskIdMapStrategyTask::reference entry)
    {
        if( entry.second->is_to_run() 
            && cur_time >= entry.second->tp_start() && cur_time < entry.second->tp_end()
            && !are_codes_in(index_codes, entry.second->code_data()) )
        { 
            strcpy(index_codes[index_count], entry.second->code_data());
            ++index_count;
        }
    });
	if( index_count < 1 )
		return;

	T_StockPriceInfo price_info[max_index_count];
	auto num = StkQuoteGetQuote_(index_codes, index_count, price_info);
	if( num < 1 )
		return;
    
	auto tp_now = std::chrono::system_clock::now();
    time_t t_t = std::chrono::system_clock::to_time_t(tp_now); 
	for( int i = 0; i < num; ++i )
	{
		auto task_ids_iter = codes_taskids_.find(index_codes[i]);
        if( task_ids_iter == codes_taskids_.end() )
            continue;
        auto quote_data = std::make_shared<QuotesData>();
		 
		quote_data->cur_price = price_info[i].cur_price;  
		quote_data->time_stamp = (__int64)t_t;
		// throu id list which this stock code related  ----------------
        std::for_each( std::begin(task_ids_iter->second), std::end(task_ids_iter->second), [&, this](unsigned int id)
        {
            TTaskIdMapStrategyTask::iterator  task_iter = registered_tasks_.find(id);
            if( task_iter != registered_tasks_.end() )
            { 
                //qDebug() << std::get<1>(CurrentDateTime()).c_str() << " " << task_iter->second->stock_code() << " cur_pric:" << task_iter->second->cur_price() << "\n";
                task_iter->second->ObtainData(quote_data);

                task_iter->second->life_count_ = 0;
				if( task_iter->second->cur_state() != TaskCurrentState::RUNNING )
				{
					task_iter->second->cur_state(TaskCurrentState::RUNNING);
					//task_iter->second->app()->Emit(task_iter->second.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
				}

            }else
            {
                // log error
                logger_.LogLocal(utility::FormatStr("error IndexTicker::Procedure can't find task %d", id));
            }
        });
	}
}
#endif

int GetRegisteredCodes(TTaskIdMapStrategyTask  &registered_tasks, char* stock_codes[cst_max_stock_code_count], byte markets[cst_max_stock_code_count])
{ 
#if 0 
    auto  cur_time = QTime::currentTime();
	//CurrentDateTime();
    //---------------------------
    int stock_count = 0;
    //std::lock_guard<std::mutex>  locker(tasks_list_mutex_);
    std::for_each( std::begin(registered_tasks), std::end(registered_tasks), [&](TTaskIdMapStrategyTask::reference entry)
    {
        if( entry.second->is_to_run() 
            && cur_time >= entry.second->tp_start() && cur_time < entry.second->tp_end()
            && !is_codes_in(stock_codes, entry.second->code_data()) )
        { 
            stock_codes[stock_count] = entry.second->code_data();
            markets[stock_count] = static_cast<byte>(entry.second->market_type());
                 
            ++stock_count;
        }
    });
    return stock_count;
#else
	return 0;
#endif
}


bool is_codes_in(char* stock_codes[cst_max_stock_code_count], const char *str)
{ 
    for(int i = 0; i < cst_max_stock_code_count; ++i)
    {
        if( stock_codes[i] == nullptr )
            return false;
        if( !strcmp(stock_codes[i], str) )
            return true;
    }
    return false;
};
