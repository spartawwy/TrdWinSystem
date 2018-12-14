#include "quotation_server_app.h"

#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>

#include "Python.h"

#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_return_code.h>
#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include <TLib/tool/tsystem_connection_handshake.pb.h>
#include <TLib/tool/tsystem_rational_number.h>
 
#include "WINNERLib/winner_message_system.h"
#include "WINNERLib/winner_msg_utility.h"

#include "file_mapping.h"


#ifdef _DEBUG
#undef Py_XDECREF
#define Py_XDECREF(a) (a)
#endif
//#pragma comment(lib, "python36.lib")
#define  GET_LONGDATE_YEAR(a) (assert(a > 10000000), a/10000)
#define  GET_LONGDATE_MONTH(a) (assert(a > 10000000), (a%10000)/100)
#define  GET_LONGDATE_DAY(a) (assert(a > 10000000), (a%100))

#define  FENBI_DATA_ALREAD

static bool IsLongDate(int date);
static bool IsStockCode(const std::string &code);
static void ConvertTime(const Time& t, int& longdate, std::string * timestamp=nullptr);
static std::vector<int> GetSpanTradeDates(int date_begin, int date_end);

static bool IsLeapYear(int year);
static int DaysOneMonth(int year, int month);

QuotationServerApp::QuotationServerApp(const std::string &name, const std::string &version)
	: ServerAppBase("quotation_server", name, version)
    , PyFuncGetAllFill2File(nullptr)
    , PyFuncGetDayKbar2File(nullptr)
    , PyFuncGetWeekKbar2File(nullptr)
    , PyFuncGetHourKbar2File(nullptr)
    , PyFuncGetRealTimeKbar(nullptr)
    , stk_data_dir_()
    , conn_strands_(256) 
    , code_fenbi_container_(5000)
{
	
}

QuotationServerApp::~QuotationServerApp()
{
	Py_Finalize();
}

void QuotationServerApp::Initiate()
{
	option_load_class_id(false);
	WINNERSystem::LoadClassID(msg_system_);
	//option_load_config(true);
	ServerAppBase::Initiate();

    for( int i = 0; i < 2; ++i )
        task_pool_.AddWorker();
 
	InitPython();

    char stk_data_dir[256] = {0};
	unsigned int ret_env_size = sizeof(stk_data_dir);
	getenv_s(&ret_env_size, stk_data_dir, ret_env_size, "STK_DATA_DIR");
    if( strlen(stk_data_dir) == 0 )
    {
        std::cout << "QuotationServerApp::Initiate STK_DATA_DIR unset! " << std::endl;
		// todo: throw exception
		throw ""; 
    } 
    std::cout << " env STK_DATA_DIR: " << stk_data_dir << std::endl;
    stk_data_dir_ = stk_data_dir;
    if( stk_data_dir_.rfind("\\") != stk_data_dir_.length() - 1 && stk_data_dir_.rfind("/") != stk_data_dir_.length() - 1 )
    {
       stk_data_dir_ += "/";
    }
    data_base_.Init();
    data_base_.LoadTradeDate(&exchange_calendar_);

	/*CreateStockTickers();*/
	SetupServerSocket();
	 
	StartPort();
}

void QuotationServerApp::InitPython()
{
	int r = Py_IsInitialized();  //1为已经初始化了
	if (r != 0)
	{
		std::cout << "QuotationServerApp::InitPython init python fail " << std::endl;
		// todo: throw exception
		throw ""; 
	}

	Py_Initialize();  
	auto p_main_Module = PyImport_ImportModule("__main__");
	if (!p_main_Module)
	{
		std::cout << "QuotationServerApp::InitPython PyImport_ImportModule __main__ fail " << std::endl;
		throw ""; 
	}
	auto pDict = PyModule_GetDict(p_main_Module);
	if ( !pDict ) 
	{          
		std::cout << "QuotationServerApp::InitPython PyModule_GetDict fail " << std::endl;
		throw ""; 
	}     

	PyRun_SimpleString("import sys");

    char stk_py_dir[256] = {0};
	unsigned int ret_env_size = sizeof(stk_py_dir);
	getenv_s(&ret_env_size, stk_py_dir, ret_env_size, "STK_PY_DIR");
    std::string path = stk_py_dir;
    std::string chdir_cmd = std::string("sys.path.append(\"") + path + "\")";
    const char* cstr_cmd = chdir_cmd.c_str(); 
    PyRun_SimpleString(cstr_cmd);

    //导入模块   fill2fill ----------------------
    PyObject* pModule = PyImport_ImportModule("get_stk_his_fill2file"); // get_stock_his.py
    if (!pModule)  
    {  
        std::cout << "QuotationServerApp::InitPython PyImport_ImportModule get_stk_his_fill2file fail " << std::endl;
		throw ""; // todo: throw exception
    }  
    PyObject *pDic = PyModule_GetDict(pModule);
    if (!pDic)  
    {  
        std::cout << "QuotationServerApp::InitPython PyModule_GetDict of get_stk_his_fill2file fail " << std::endl;
		throw ""; // todo: throw exception
    }  

    PyObject* pcls = PyObject_GetAttrString(pModule, "STOCK");   
    if (!pcls || !PyCallable_Check(pcls))  
    {  
        std::cout << "QuotationServerApp::InitPython PyObject_GetAttrString class STOCK fail " << std::endl;
		throw ""; // todo: throw exception
    }  
 
    PyObject* p_stock_obj = PyEval_CallObject(pcls, NULL);
    if( !p_stock_obj )
    { 
        std::cout << "QuotationServerApp::InitPython PyEval_CallObject create STOCK obj fail " << std::endl;
		throw ""; // todo: throw exception
    }
    PyFuncGetAllFill2File = PyObject_GetAttrString(p_stock_obj, "getAllFill2File");
    if( !PyFuncGetAllFill2File )
    {
        std::cout << "QuotationServerApp::InitPython get func getAllFill2File fail " << std::endl;
		throw ""; // todo: throw exception
    }

    //导入模块   get_stk_kline ----------------------
    PyObject* pModule_kline = PyImport_ImportModule("get_stk_kline"); // get_stock_his.py
    if (!pModule)  
    {  
        std::cout << "QuotationServerApp::InitPython PyImport_ImportModule get_stk_kline fail " << std::endl;
		throw ""; // todo: throw exception
    }  
    PyObject *pDic_kline = PyModule_GetDict(pModule_kline);
    if (!pDic_kline)  
    {  
        std::cout << "QuotationServerApp::InitPython PyModule_GetDict of get_stk_kline fail " << std::endl;
		throw ""; // todo: throw exception
    }  

    PyObject* pcls_kline = PyObject_GetAttrString(pModule_kline, "KLINE");   
    if (!pcls_kline || !PyCallable_Check(pcls_kline))  
    {  
        std::cout << "QuotationServerApp::InitPython PyObject_GetAttrString class STOCK fail " << std::endl;
		throw ""; // todo: throw exception
    }  
 
    PyObject* p_kline_obj = PyEval_CallObject(pcls_kline, NULL);
    if( !p_stock_obj )
    { 
        std::cout << "QuotationServerApp::InitPython PyEval_CallObject create STOCK obj fail " << std::endl;
		throw ""; // todo: throw exception
    }

    PyFuncGetDayKbar2File = PyObject_GetAttrString(p_kline_obj, "getDayKBarData");
    if( !PyFuncGetDayKbar2File )
    {
        std::cout << "QuotationServerApp::InitPython get func getDayKBarData fail " << std::endl;
		throw ""; // todo: throw exception
    }
    PyFuncGetWeekKbar2File = PyObject_GetAttrString(p_kline_obj, "getWeekKBarData");
    if( !PyFuncGetWeekKbar2File )
    {
        std::cout << "QuotationServerApp::InitPython get func getWeekKBarData fail " << std::endl;
        throw ""; // todo: throw exception
    } 
    PyFuncGetHourKbar2File = PyObject_GetAttrString(p_kline_obj, "getHourKBarData");
    if( !PyFuncGetHourKbar2File )
    {
        std::cout << "QuotationServerApp::InitPython get func getHourKBarData fail " << std::endl;
        throw ""; // todo: throw exception
    } 
    PyFuncGetRealTimeKbar = PyObject_GetAttrString(p_kline_obj, "get_realtime_k_data");
    if( !PyFuncGetRealTimeKbar )
    {
        std::cout << "QuotationServerApp::InitPython get func get_realtime_k_data fail " << std::endl;
		throw ""; // todo: throw exception
    }
}


void QuotationServerApp::HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg)
{
    HandShake hs;

	if(Decode(msg, hs, msg_system_))
	{
		const layinfo::AppResource& res = resource_manager_.FindUnique(hs.pid()); 
		if( res )
		{
			auto pconn = p->shared_this();

			p->hand_shaked(true);
            printf("authorized connection %d, sending heat beat and handshake: %d(%s)\n", pconn->connid(), res.pid, res.name.c_str());
			SetupInboundConnection(pconn, res);

			// add connection for forward module
			//this->forward_module_.AddConnection(hs.pid(), pconn);

			// add connection for master selection module
			//this->master_selection_module_.AsyncAddResource(res.raid_id, res.pid, pconn);
		}
	}
}

void QuotationServerApp::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) 
{
    WriteLock locker(conn_strands_wr_mutex_);
    printf("HandleInboundDisconnect connid:%d disconnect\n", pconn->connid());
    this->local_logger().LogLocal(utility::FormatStr("HandleInboundDisconnect connid:%d disconnect", pconn->connid()));
    auto iter = conn_strands_.find(pconn->connid());
    if( iter != conn_strands_.end() )
    {
        conn_strands_.erase(iter);
    }
}

void QuotationServerApp::UpdateState()
{

}

void QuotationServerApp::SetupServerSocket()
{
	msg_handlers_.RegisterHandler("UserRequest", [this](communication::Connection* p, const Message& msg)
	{
		auto req = std::make_shared<UserRequest>();
		if( Decode(msg, *req, this->msg_system()) )
			this->task_pool_.PostTask( std::bind(&QuotationServerApp::HandleUserRequest, this, std::move(req), p->shared_this()));
	});

    msg_handlers_.RegisterHandler("QuotationRequest", [this](communication::Connection* p, const Message& msg)
    {
        auto req = std::make_shared<QuotationRequest>();
        if( Decode(msg, *req, this->msg_system()) )
        {
            WriteLock locker(conn_strands_wr_mutex_);
            auto iter = conn_strands_.find(p->connid());
            if( iter == conn_strands_.end() )
            {
                iter = conn_strands_.insert( std::make_pair(p->connid(), std::make_shared<TaskStrand>(task_pool())) ).first;
            } 
            iter->second->PostTask( std::bind(&QuotationServerApp::HandleQuotationRequest, this, std::move(req), p->shared_this()));
             
        }
    });
}

void QuotationServerApp::HandleUserRequest(std::shared_ptr<UserRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
	switch (req->request_type())
	{
	case RequestType::LOGIN:
		HandleUserLogin(*req, pconn);
		break;
	case RequestType::LOGOUT:
		//HandleUserLogout(*req, pconn);
		break;
	default:
		break;
	}
}

void QuotationServerApp::HandleUserLogin(const UserRequest& req, const std::shared_ptr<communication::Connection>& pconn)
{
	SendUserRequestAck(req.user_id(), req.request_id(), req.request_type(), pconn);
}

void QuotationServerApp::HandleQuotationRequest(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
     printf("\nconn:%d HandleQuotationRequest: %s\n", pconn->connid(), WINNERSystem::QuotationRequest2Str(*req).c_str());
     switch( req->req_type() )
     {
     case QuotationReqType::FENBI:
        _HandleQuotatoinFenbi(req, pconn);
         break;
     case QuotationReqType::ONE_MINUTE: 
         SendRequestAck(req, 2, "no related data", pconn);
         break;
     case QuotationReqType::FIVE_MINUTE: // 5 minute
     case QuotationReqType::FIFTEEN_MINUTE:
     case QuotationReqType::THIRTY_MINUTE:
     case QuotationReqType::HOUR:
     case QuotationReqType::DAY:
     case QuotationReqType::WEEK:
     case QuotationReqType::MONTH:
         _HandleQuotatoinKbar(req, pconn);
         break;
     case QuotationReqType::HIS_QUOTE:
         _HandleQuotatoinHisQuote(req, pconn);
         break;
     }

}

void QuotationServerApp::_HandleQuotatoinFenbi(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
   assert(req);
   assert(pconn); 
      
    static auto create_file_path = [](const std::string& stk_data_dir, const std::string& date_str, const std::string& code)
    {
         int date = bar_daystr_to_longday(date_str);
	     auto date_com = TSystem::FromLongdate(date);
         std::string date_format_str = utility::FormatStr("%d%02d%02d", std::get<0>(date_com), std::get<1>(date_com), std::get<2>(date_com));
	     std::string year_mon = utility::FormatStr("%d%02d", std::get<0>(date_com), std::get<1>(date_com));
         std::string year_mon_sub = year_mon + (IsShStock(code) ? "SH" : "SZ");
         return stk_data_dir + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + code + "_" + date_format_str + ".csv";
    };
  
    static auto fetch_data_send 
        = [this](std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn, int begin_date, int end_date, TDayFenbi &days_fenbi, bool is_last)
    { 
#ifndef FENBI_DATA_ALREAD 
        auto ret_date_str_vector = GetFenbi2File(req->code(), begin_date, end_date);
        std::for_each( std::begin(ret_date_str_vector), std::end(ret_date_str_vector), [&req, &pconn, is_last, this](const std::string &entry)
        { 
            std::string full_path = this->stk_data_dir_ + req->code() + "/" + entry + "/fenbi/" + req->code() + ".fenbi";
            QuotationMessage quotation_msg;
            quotation_msg.set_code(req->code());
            quotation_msg.set_req_id(req->req_id);
            if( is_last )
                quotation_msg.set_is_last(is_last);
#else 
        auto date_vector = GetSpanTradeDates(begin_date, end_date);
        std::for_each( std::begin(date_vector), std::end(date_vector), [&req, &pconn, &days_fenbi, is_last, this](const int date)
        { 
            if( !this->exchange_calendar_.IsTradeDate(date) )
                return;
            QuotationMessage quotation_msg;
            quotation_msg.set_code(req->code());
            quotation_msg.set_req_id(req->req_id());
            if( is_last )
                quotation_msg.set_is_last(is_last);
            auto day_iter = days_fenbi.find(date);
            if( day_iter != days_fenbi.end() )
            { 
                std::for_each( std::begin(*(day_iter->second)), std::end(*(day_iter->second)), [&quotation_msg, date, &pconn, this](T_Fenbi& fenbi)
                {
                    QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();
                    auto date_comp = TSystem::FromLongdate(date);
                    FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                        , GET_HOUR(fenbi.hhmmss), GET_MINUTE(fenbi.hhmmss), GET_SECOND(fenbi.hhmmss)))
                        , *p_fill_msg->mutable_time() ); 
                    char tmp_buf[16];
                    sprintf_s(tmp_buf, sizeof(tmp_buf), "%.2f", fenbi.price);
                    TSystem::FillRational(std::string(tmp_buf), *p_fill_msg->mutable_price()); 
                    TSystem::FillRational(std::to_string(0), *p_fill_msg->mutable_price_change()); //ndedt
                    p_fill_msg->set_is_change_positive(false);//ndedt
                    p_fill_msg->set_vol(1);
                });
                printf("pconn->AsyncSend\n");
                pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
                return;
            } 
            auto fenbi_vector = std::make_shared<std::vector<T_Fenbi>>();
            fenbi_vector->reserve(1024);
            day_iter = days_fenbi.insert( std::make_pair(date, std::move(fenbi_vector)) ).first;

            std::string date_format_str = utility::FormatStr("%d", date);
	        std::string year_mon = utility::FormatStr("%d%02d", (date/10000), date%10000/100);
            std::string year_mon_sub = year_mon + (IsShStock(req->code()) ? "SH" : "SZ");
            std::string full_path = this->stk_data_dir_ + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + req->code() + "_" + date_format_str + ".csv";
#endif
            //std::string id;
            std::string hour;
            std::string minute;
            std::string second;
            std::string price;
            std::string change_price;
            std::string vol;
            //std::string amount; 

            std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}):(\\d{2}):(\\d{2}),(\\d+\\.\\d+),(\\d+)(.*)$"; 
            std::regex regex_obj(partten_string); 
            
#ifdef FENBI_DATA_ALREAD
             
             char buf[256] = {0};
             std::fstream in(full_path);
             if( !in.is_open() )
             {
                 printf("open file fail");
                 getchar();
                 return;
             }
             int line = 0;
             while( !in.eof() )
             {
                 if( line++ % 100 == 0 )
                     Delay(1);
                 in.getline(buf, sizeof(buf));
                 int len = strlen(buf);
                 if( len < 1 )
                     continue;
                 char *p0 = buf;
                 char *p1 = &buf[len-1];
                 std::string src(p0, p1);

                std::smatch result; 
                if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
                {  //std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
                    hour = result[2];
                    minute = result[3];
                    second = result[4];
                    price = result[5];
                    change_price = "0.0";
                    vol =  result[6];
                    try
                    {
                        QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();
                     
                        auto date_comp = TSystem::FromLongdate(date);
                        FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                            , std::stoi(hour), std::stoi(minute), std::stoi(second)))
                            , *p_fill_msg->mutable_time() );
                        TSystem::FillRational(price, *p_fill_msg->mutable_price()); 
                        TSystem::FillRational(change_price, *p_fill_msg->mutable_price_change());

                        if( std::stod(change_price) < 0.0 )
                            p_fill_msg->set_is_change_positive(false);
                        p_fill_msg->set_vol(std::stoi(vol)); 

                        // push to code_fenbi_container
                        int hhmmss = std::stoi(hour)*10000 + std::stoi(minute)*100 + std::stoi(second);
                        day_iter->second->push_back(std::move(T_Fenbi(hhmmss, std::stof(price))));

                    }catch(std::exception &e)
                    {
                        printf("exception:%s", e.what());
                        continue;
                    }catch(...)
                    {
                        continue;
                    }
                }
                //--------------------
                if( *p1 == '\0' ) break; 
                if( int(*p1) == 0x0A ) // filter 0x0A
                    ++p1;
            }

#else
            char strbuf[1024] = {0};
            while( *p1 != '\0' ) 
            {
                int count = 0;
                p0 = p1;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                // time realte -----
                p0 = ++p1;
                count = 0;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                if( count > 0 )
                { 
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0';   
                    strbuf[2] = '\0';
                    hour = strbuf;
                    strbuf[5] = '\0';
                    minute = strbuf + 3; 
                    second = strbuf + 6;
                }
         
                // price realte -----
                p0 = ++p1;
                count = 0;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                if( count > 0 )
                { 
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0'; 
                    price = strbuf;
                }
                // change_price realte -----
                p0 = ++p1;
                count = 0;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                if( count > 0 )
                { 
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0'; 
                    change_price = strbuf;
                }
                // vol realte -----
                p0 = ++p1;
                count = 0;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                if( count > 0 )
                { 
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0'; 
                    vol = strbuf;
                }
                // amount realte -----
                p0 = ++p1;
                count = 0;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                // filter 0x0A
                if( *p1 != '\0' )
                    ++p1;
                try
                {
                QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();

                int longdate = bar_daystr_to_longday(entry);
                auto date_comp = TSystem::FromLongdate(longdate);
                FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                    , std::stoi(hour), std::stoi(minute), std::stoi(second)))
                    , *p_fill_msg->mutable_time() );
                TSystem::FillRational(price, *p_fill_msg->mutable_price()); 
                TSystem::FillRational(change_price, *p_fill_msg->mutable_price_change());
            
                if( std::stod(change_price) < 0.0 )
                    p_fill_msg->set_is_change_positive(false);

                p_fill_msg->set_vol(std::stoi(vol));
                }catch(std::exception &e)
                {
                    printf("exception:%s", e.what());
                    continue;
                }catch(...)
                {
                    continue;
                }

                //std::cout <<  price << " " << change_price << " " << vol << std::endl;
            } // while 
#endif
            printf("pconn->AsyncSend\n");
            pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 

        }); // std::for_each file

    }; // fetch_data_send

    auto fenbi_code_iter = code_fenbi_container_.find(req->code());
    if( fenbi_code_iter == code_fenbi_container_.end() )
        fenbi_code_iter = code_fenbi_container_.insert(std::make_pair(req->code(), TDayFenbi(10*20))).first; // 10 months
    TDayFenbi &days_fenbi = fenbi_code_iter->second;

    auto tm_point = TSystem::MakeTimePoint((time_t)req->beg_time().time_value(), req->beg_time().frac_sec());
    auto tm_point_end = TSystem::MakeTimePoint((time_t)req->end_time().time_value(), req->end_time().frac_sec());
 
    TimePoint t_p(tm_point);
    int longday_beg = ToLongdate(t_p.year(), t_p.month(), t_p.day());
    
    TimePoint t_p_end(tm_point_end);
    int longday_end = ToLongdate(t_p_end.year(), t_p_end.month(), t_p_end.day());

    auto date_vector = GetSpanTradeDates(longday_beg, longday_end);

    const int span_len = 5;
    unsigned int i = 0;
    for( ; i < date_vector.size() / span_len; ++i )
    { 
       fetch_data_send(req, pconn, date_vector[i*span_len], date_vector[(i + 1)*span_len - 1], days_fenbi, (i + 1)*span_len==date_vector.size());
       if( i < 10 )
           Delay(20);
       else if(i < 20 )
           Delay(40);
       else                                                                                                                                                                                                                                                                                                                                                                                                                                                          
           Delay(80); 
    }
     
    if( date_vector.size() % span_len  )
    {
        fetch_data_send(req, pconn, date_vector[i*span_len], date_vector[date_vector.size() - 1], days_fenbi, true);
    }
}

void QuotationServerApp::_HandleQuotatoinKbar(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
    auto tm_point = TSystem::MakeTimePoint((time_t)req->beg_time().time_value(), req->beg_time().frac_sec());
    auto tm_point_end = TSystem::MakeTimePoint((time_t)req->end_time().time_value(), req->end_time().frac_sec());

    TimePoint t_p(tm_point);
    int longday_beg = ToLongdate(t_p.year(), t_p.month(), t_p.day());

    TimePoint t_p_end(tm_point_end);
    int longday_end = ToLongdate(t_p_end.year(), t_p_end.month(), t_p_end.day());

    switch( req->req_type() )
    {
    case QuotationReqType::DAY:
        {
            _HandleQuotatoinKBarDay(req, pconn, req->code(), longday_beg, longday_end, req->fq_type(), req->is_index());
            break;
        }
    case QuotationReqType::WEEK:
        {
            _HandleQuotatoinKBarWeek(req, pconn, req->code(), longday_beg, longday_end, req->fq_type(), req->is_index());
            break;
        } 
    case QuotationReqType::HOUR:
        {
            _HandleQuotatoinKBarHour(req, pconn, req->code(), longday_beg, longday_end, req->fq_type(), req->is_index());
            break;
        }
    }
}

void QuotationServerApp::_HandleQuotatoinKBarDay(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection> &pconn, const std::string &code, int beg_date, int end_date, QuotationFqType fqtye, bool is_index)
{
    std::vector<std::string> ret_date_str_vector = GetDayKbars2File(code, beg_date, end_date, fqtye, is_index);
    QuotationMessage quotation_msg;
    quotation_msg.set_req_id(req->req_id());
    quotation_msg.set_is_last(true);
    quotation_msg.set_code(code);
    std::string temp_code = code;
    if( is_index && code == "000001" )
        temp_code = "999999";
    const std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+)(.*)$"; 
    ReadQuoteMessage(ret_date_str_vector, temp_code, KLINE_TYPE::KTYPE_DAY, beg_date, end_date, partten_string, quotation_msg);
 
     
    if( end_date >= TSystem::Today() )
    {
        QuotationMessage::QuotationKbarMessage kbar_msg;
        GetRealTimeK(code, kbar_msg, is_index);
        if( quotation_msg.kbar_msgs_size() > 0 )
        {
            auto lastst = quotation_msg.mutable_kbar_msgs()->Mutable(quotation_msg.kbar_msgs_size()-1);
            if( RationalDouble(kbar_msg.high()) > RationalDouble(lastst->high()) )
                *lastst->mutable_high() = kbar_msg.high();
            if( RationalDouble(kbar_msg.low()) < RationalDouble(lastst->low()) )
                *lastst->mutable_low() = kbar_msg.low();
            *lastst->mutable_close() = kbar_msg.close();
            // todo:vol
        }
    }
     
    if( quotation_msg.kbar_msgs().size() > 0 )
    { 
        printf("pconn->AsyncSend\n");
        pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
    }else
    { 
        RequestAck ack;
        FillRequestAck(req->req_id(), 2, "no related data", ack);
        pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
    }
}

void QuotationServerApp::_HandleQuotatoinKBarWeek(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection> &pconn, const std::string &code, int beg_date, int end_date, QuotationFqType fqtye, bool is_index)
{
    std::vector<std::string> ret_date_str_vector = GetWeekKbars2File(code, beg_date, end_date, fqtye, is_index);
    QuotationMessage quotation_msg;
    quotation_msg.set_req_id(req->req_id());
    quotation_msg.set_is_last(true);
    quotation_msg.set_code(code);
    std::string temp_code = code;
    if( is_index && code == "000001" )
        temp_code = "999999";
    const std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+)(.*)$"; 
    ReadQuoteMessage(ret_date_str_vector, temp_code, KLINE_TYPE::KTYPE_WEEK, beg_date, end_date, partten_string, quotation_msg);
    if( IsSameWeek(end_date, Today()) )
    {
        QuotationMessage::QuotationKbarMessage kbar_msg;
        GetRealTimeK(code, kbar_msg, is_index);
        if( quotation_msg.kbar_msgs_size() > 0 )
        {
            auto lastst = quotation_msg.mutable_kbar_msgs()->Mutable(quotation_msg.kbar_msgs_size()-1);
            if( RationalDouble(kbar_msg.high()) > RationalDouble(lastst->high()) )
                *lastst->mutable_high() = kbar_msg.high();
            if( RationalDouble(kbar_msg.low()) < RationalDouble(lastst->low()) )
                *lastst->mutable_low() = kbar_msg.low();
            *lastst->mutable_close() = kbar_msg.close();
            // todo:vol
        }
    }

    if( quotation_msg.kbar_msgs().size() > 0 )
    { 
        printf("pconn->AsyncSend\n");
        pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
    }else
    { 
        RequestAck ack;
        FillRequestAck(req->req_id(), 2, "no related data", ack);
        pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
    }
}

void QuotationServerApp::_HandleQuotatoinKBarHour(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection> &pconn, const std::string &code, int beg_date, int end_date, QuotationFqType fqtye, bool is_index)
{
    std::vector<std::string> ret_file_str_vector = GetHourKbars2File(code, beg_date, end_date, fqtye, is_index);
    QuotationMessage quotation_msg;
    quotation_msg.set_req_id(req->req_id());
    quotation_msg.set_is_last(true);
    quotation_msg.set_code(code);
    std::string temp_code = code;
    if( is_index && code == "000001" )
        temp_code = "999999";
    const std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}) (\\d{1,2}):(\\d{1,2}),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+)(.*)$"; 
    ReadQuoteMessage(ret_file_str_vector, temp_code, KLINE_TYPE::KTYPE_HOUR, beg_date, end_date, partten_string, quotation_msg);

    if( quotation_msg.kbar_msgs().size() > 0 )
    { 
        printf("pconn->AsyncSend\n");
        pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
    }else
    {  
        SendRequestAck(req, 2, "no related data", pconn);
    } 
}

void QuotationServerApp::_HandleQuotatoinHisQuote(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
    assert(req);
    assert(pconn); 

    auto tm_point = TSystem::MakeTimePoint((time_t)req->beg_time().time_value(), req->beg_time().frac_sec()); 
    TimePoint t_p(tm_point);
    const int date = ToLongdate(t_p.year(), t_p.month(), t_p.day());
      
    static auto create_file_path = [](const std::string& stk_data_dir, const std::string& date_str, const std::string& code)
    {
         int date = bar_daystr_to_longday(date_str);
	     auto date_com = TSystem::FromLongdate(date);
         std::string date_format_str = utility::FormatStr("%d%02d%02d", std::get<0>(date_com), std::get<1>(date_com), std::get<2>(date_com));
	     std::string year_mon = utility::FormatStr("%d%02d", std::get<0>(date_com), std::get<1>(date_com));
         std::string year_mon_sub = year_mon + (IsShStock(code) ? "SH" : "SZ");
         return stk_data_dir + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + code + "_" + date_format_str + ".csv";
    };
   
    if( !this->exchange_calendar_.IsTradeDate(date) )
        return;
    QuotationMessage quotation_msg;
    quotation_msg.set_code(req->code());
    quotation_msg.set_req_id(req->req_id());
             
    /*auto fenbi_vector = std::make_shared<std::vector<T_Fenbi>>();
    fenbi_vector->reserve(1024);*/
             
    std::string date_format_str = utility::FormatStr("%d", date);
	std::string year_mon = utility::FormatStr("%d%02d", (date/10000), date%10000/100);
    std::string year_mon_sub = year_mon + (IsShStock(req->code()) ? "SH" : "SZ");
    std::string full_path = this->stk_data_dir_ + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + req->code() + "_" + date_format_str + ".csv";
#if 0
    std::string hour;
    std::string minute;
    std::string second;
    std::string price;
    
    std::string vol;
    std::string b_1, b_2, b_3, b_4, b_5;
    std::string s_1, s_2, s_3, s_4, s_5;
    //std::string amount; 
#endif
    std::string change_price;
    /////////////////////////////////
    // read quotes from a file
    /////////////////////////////////
     
    std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}):(\\d{2}):(\\d{2}),(\\d+\\.\\d+),(\\d+),(\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+),(\\d+\\.\\d+),(\\d+)(.*)$";
    std::regex regex_obj(partten_string); 
              
    char buf[256] = {0};
    std::fstream in(full_path);
    if( !in.is_open() )
    {
        printf("open file fail");
        getchar();
        return;
    } 

    int line = 0;
    while( !in.eof() )
    {
        in.getline(buf, sizeof(buf));
        
        int len = strlen(buf);
        if( len < 1 )
            continue;
        char *p0 = buf;
        char *p1 = &buf[len-1];
        std::string src(p0, p1);

        std::smatch result; 
        if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
        {  //std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl; 
#if 0
            hour = result[2];
            minute = result[3];
            second = result[4];
            price = result[5];
            vol =  result[6];

            b_1 = result[9];
            b_2 = result[11];
            b_3 = result[13];
            b_4 = result[15];
            b_5 = result[17];
            s_1 = result[19];
            s_2 = result[21];
            s_3 = result[23];
            s_4 = result[25];
            s_5 = result[27];
#endif
            change_price = "0.0";
            try
            {
                QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();
                     
                auto date_comp = TSystem::FromLongdate(date);
                FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                    , std::stoi(result[2]), std::stoi(result[3]), std::stoi(result[4])))
                    , *p_fill_msg->mutable_time() );
                time_t time_val = p_fill_msg->time().time_value();
                TSystem::FillRational(result[5], *p_fill_msg->mutable_price()); 
                //TSystem::FillRational(change_price, *p_fill_msg->mutable_price_change());

                if( std::stod(change_price) < 0.0 )
                    p_fill_msg->set_is_change_positive(false);
                p_fill_msg->set_vol(std::stoi(result[6])); 

                // push to code_fenbi_container
                //int hhmmss = std::stoi(result[2])*10000 + std::stoi(result[3])*100 + std::stoi(result[4]);

                TSystem::FillRational(result[9], *p_fill_msg->mutable_b_1());
                TSystem::FillRational(result[11], *p_fill_msg->mutable_b_2());
                TSystem::FillRational(result[13], *p_fill_msg->mutable_b_3());
                TSystem::FillRational(result[15], *p_fill_msg->mutable_b_4());
                TSystem::FillRational(result[17], *p_fill_msg->mutable_b_5());
                TSystem::FillRational(result[19], *p_fill_msg->mutable_s_1());
                TSystem::FillRational(result[21], *p_fill_msg->mutable_s_2());
                TSystem::FillRational(result[23], *p_fill_msg->mutable_s_3());
                TSystem::FillRational(result[25], *p_fill_msg->mutable_s_4());
                TSystem::FillRational(result[27], *p_fill_msg->mutable_s_5());
                 
                if( line++ % 100 == 0 )
                {   
                    if( pconn->status() != TSystem::communication::Connection::Status::connected )
                        return;
                    printf("pconn->AsyncSend quote message\n");
                    pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
                    quotation_msg.mutable_quote_fill_msgs()->Clear();
                    
                    Delay(2);
                }

            }catch(std::exception &e)
            {
                printf("exception:%s", e.what());
                continue;
            }catch(...)
            {
                continue;
            }
        }
        //--------------------
        if( *p1 == '\0' ) break; 
        if( int(*p1) == 0x0A ) // filter 0x0A
            ++p1;
    } // while
     
    quotation_msg.set_is_last(true);
    printf("pconn->AsyncSend quote message\n");
    pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 
}


void QuotationServerApp::ReadQuoteMessage(const std::vector<std::string> &ret_date_str_vector, const std::string &code, KLINE_TYPE k_type, int beg_date, int end_date, const std::string &partten_str, QuotationMessage &quotation_msg)
{
    for( auto entry = ret_date_str_vector.begin(); entry < ret_date_str_vector.end(); ++entry )
    { 
        std::string full_path = this->stk_data_dir_ + code + "/kline/" + *entry;

       // std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+\\.\\d+),(\\d+)(.*)$"; 
        std::regex regex_obj(partten_str); 
        char buf[256] = {0};
        std::fstream in(full_path);
        if( !in.is_open() )
        {
            printf("ReadQuoteMessage open file fail");
            continue;
        }
        int line = 0;
        while( !in.eof() )
        {
            if( line++ % 20 == 0 )
                Delay(5);
            in.getline(buf, sizeof(buf));
            int len = strlen(buf);
            if( len < 1 )
                continue;
            char *p0 = buf;
            char *p1 = &buf[len-1];
            std::string src(p0, p1);
            std::smatch result; 
            if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
            {  //std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
                std::string hour_str;
                std::string minute_str;
                int index = 0;
                std::string date_str = result[++index];
                int date_val = bar_daystr_to_longday(date_str);
                if( date_val < beg_date )
                    continue;
                if( k_type == KLINE_TYPE::KTYPE_HOUR )
                {
                    hour_str = result[++index];
                    minute_str = result[++index]; 
                }
                std::string open_str = result[++index];
                std::string close_str = result[++index];
                std::string high_str = result[++index];
                std::string low_str = result[++index];
                std::string vol_str = result[++index];
                
                if( date_val > end_date )
                {
                    if( k_type == KLINE_TYPE::KTYPE_DAY )
                    { 
                        return;
                    }else if( k_type == KLINE_TYPE::KTYPE_WEEK )
                    {
                        if( !IsSameWeek(date_val, end_date) )
                            return;
                    }
                }
                try
                {
                    QuotationMessage::QuotationKbarMessage * p_kbar_msg = quotation_msg.add_kbar_msgs();
                    if( k_type == KLINE_TYPE::KTYPE_HOUR )
                    {
                        int hhmmss = boost::lexical_cast<int>(hour_str) * 10000 + boost::lexical_cast<int>(minute_str) * 100;
                        p_kbar_msg->set_hhmmdd(hhmmss);
                    }
                    p_kbar_msg->set_yyyymmdd(date_val);
                    TSystem::FillRational(open_str, *p_kbar_msg->mutable_open()); 
                    TSystem::FillRational(close_str, *p_kbar_msg->mutable_close());
                    TSystem::FillRational(high_str, *p_kbar_msg->mutable_high());
                    TSystem::FillRational(low_str, *p_kbar_msg->mutable_low());
                    TSystem::FillRational(vol_str, *p_kbar_msg->mutable_vol());
                }catch(std::exception &e)
                {
                    printf("exception:%s", e.what());
                    continue;
                }catch(...)
                {
                    continue;
                }
            }
            //--------------------
            if( *p1 == '\0' ) break; 
            if( int(*p1) == 0x0A ) // filter 0x0A
                ++p1;
        } // while
    }// for each file
}


void QuotationServerApp::SendUserRequestAck(int user_id, int req_id, RequestType type, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	UserRequestAck ack;
	ack.set_req_type(type);
	ack.set_user_id(user_id);
	TSystem::FillRequestAck(req_id, *ack.mutable_req_ack());
	pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
}

void QuotationServerApp::SendRequestAck(const std::shared_ptr<QuotationRequest>& req, int error_code, const std::string & error_info, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
    RequestAck ack;
    FillRequestAck(req->req_id(), error_code, error_info, ack);
    pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
}

std::vector<std::string> QuotationServerApp::GetFenbi2File(const std::string &code, int date_beg, int date_end)
{
#ifdef FENBI_DATA_ALREAD
    return std::vector<std::string>();

#else
    assert(PyFuncGetAllFill2File);

    if( !IsLongDate(date_beg) || !IsLongDate(date_end) || !IsStockCode(code) )
        return std::vector<std::string>();
    int beg_date = (date_beg > date_end) ? date_end : date_beg;
    int end_date = (date_beg > date_end) ? date_beg : date_end;
     
    auto pArg = PyTuple_New(3);
    PyTuple_SetItem(pArg, 0, Py_BuildValue("s", code.c_str()));
    char beg_date_str[32] = {0};
    sprintf_s(beg_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(beg_date), GET_LONGDATE_MONTH(beg_date), GET_LONGDATE_DAY(beg_date));
    PyTuple_SetItem(pArg, 1, Py_BuildValue("s", beg_date_str));
    char end_date_str[32] = {0};
    sprintf_s(end_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(end_date), GET_LONGDATE_MONTH(end_date), GET_LONGDATE_DAY(end_date));
    PyTuple_SetItem(pArg, 2, Py_BuildValue("s", end_date_str));
    
    std::vector<std::string> ret_vector;
    char *result;
    try
    {
    auto pRet = PyEval_CallObject((PyObject*)PyFuncGetAllFill2File, pArg);
    if( !pRet )
    {
        std::cout << " GetFenbi2File  PyFuncGetAllFill2File ret null! beg_date:" << beg_date << " end_date:" << end_date << std::endl;
        local_logger().LogLocal(utility::FormatStr("GetFenbi2File  PyFuncGetAllFill2File ret null! beg_date:%d end_date:%d", beg_date, end_date));
        return ret_vector;
    }
    PyArg_Parse(pRet, "s", &result);
    
    if( result && strlen(result) > 0 )
    {
        if( strstr(result, ";") )
        {
            ret_vector = utility::split(result, ";"); 
        }else
            ret_vector.push_back(result);
        Py_XDECREF(result);
    } 
    }catch(...)
    {
        Py_XDECREF(result);
    } 
    //-------------------
    return ret_vector; 
#endif
}

std::vector<std::string> QuotationServerApp::GetDayKbars2File(const std::string &code, int date_beg, int date_end
                        ,  QuotationFqType fq_type, bool is_index)
{
    assert(PyFuncGetDayKbar2File);

    if( !IsLongDate(date_beg) || !IsLongDate(date_end) || !IsStockCode(code) )
        return std::vector<std::string>();
    int beg_date = (date_beg > date_end) ? date_end : date_beg;
    int end_date = (date_beg > date_end) ? date_beg : date_end;
     
    auto pArg = PyTuple_New(4);
    PyTuple_SetItem(pArg, 0, Py_BuildValue("s", code.c_str()));
    char beg_date_str[32] = {0};
    sprintf_s(beg_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(beg_date), GET_LONGDATE_MONTH(beg_date), GET_LONGDATE_DAY(beg_date));
    PyTuple_SetItem(pArg, 1, Py_BuildValue("s", beg_date_str));
    char end_date_str[32] = {0};
    sprintf_s(end_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(end_date), GET_LONGDATE_MONTH(end_date), GET_LONGDATE_DAY(end_date));
    PyTuple_SetItem(pArg, 2, Py_BuildValue("s", end_date_str));

    PyTuple_SetItem(pArg, 3, Py_BuildValue("b", is_index));

    std::vector<std::string> ret_vector;
    char *result;
    try
    {
    auto pRet = PyEval_CallObject((PyObject*)PyFuncGetDayKbar2File, pArg);
    if( !pRet )
    {
        std::cout << " GetDayKbars2File  PyFuncGetDayKbar2File ret null! beg_date:" << beg_date << " end_date:" << end_date << std::endl;
        local_logger().LogLocal(utility::FormatStr("GetDayKbars2File  PyFuncGetDayKbar2File ret null! beg_date:%d end_date:%d", beg_date, end_date));
        return ret_vector;
    }
    PyArg_Parse(pRet, "s", &result);
    
    if( result && strlen(result) > 0 )
    {
        if( strstr(result, ";") )
        {
            ret_vector = utility::split(result, ";"); 
        }else
            ret_vector.push_back(result);
        Py_XDECREF(result);
    } 
    }catch(...)
    {
        Py_XDECREF(result);
    } 
    //-------------------
    return ret_vector; 
}

std::vector<std::string> QuotationServerApp::GetWeekKbars2File(const std::string &code, int date_beg, int date_end
                                           ,  QuotationFqType fq_type, bool is_index)
{
    assert(PyFuncGetWeekKbar2File);

    if( !IsLongDate(date_beg) || !IsLongDate(date_end) || !IsStockCode(code) )
        return std::vector<std::string>();
    int beg_date = (date_beg > date_end) ? date_end : date_beg;
    int end_date = (date_beg > date_end) ? date_beg : date_end;

    auto pArg = PyTuple_New(4);
    PyTuple_SetItem(pArg, 0, Py_BuildValue("s", code.c_str()));
    char beg_date_str[32] = {0};
    sprintf_s(beg_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(beg_date), GET_LONGDATE_MONTH(beg_date), GET_LONGDATE_DAY(beg_date));
    PyTuple_SetItem(pArg, 1, Py_BuildValue("s", beg_date_str));
    char end_date_str[32] = {0};
    sprintf_s(end_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(end_date), GET_LONGDATE_MONTH(end_date), GET_LONGDATE_DAY(end_date));
    PyTuple_SetItem(pArg, 2, Py_BuildValue("s", end_date_str));

    PyTuple_SetItem(pArg, 3, Py_BuildValue("b", is_index));

    std::vector<std::string> ret_vector;
    char *result;
    try
    {
        auto pRet = PyEval_CallObject((PyObject*)PyFuncGetWeekKbar2File, pArg);
        if( !pRet )
        {
            std::cout << " PyFuncGetWeekKbar2File  ret null! beg_date:" << beg_date << " end_date:" << end_date << std::endl;
            local_logger().LogLocal(utility::FormatStr("PyFuncGetWeekKbar2File   ret null! beg_date:%d end_date:%d", beg_date, end_date));
            return ret_vector;
        }
        PyArg_Parse(pRet, "s", &result);

        if( result && strlen(result) > 0 )
        {
            if( strstr(result, ";") )
            {
                ret_vector = utility::split(result, ";"); 
            }else
                ret_vector.push_back(result);
            Py_XDECREF(result);
        } 
    }catch(...)
    {
        Py_XDECREF(result);
    } 
    //-------------------
    return ret_vector; 
}

std::vector<std::string> QuotationServerApp::GetHourKbars2File(const std::string &code, int date_beg, int date_end
                                           ,  QuotationFqType fq_type, bool is_index)
{ 
    assert(PyFuncGetHourKbar2File);

    if( !IsLongDate(date_beg) || !IsLongDate(date_end) || !IsStockCode(code) )
        return std::vector<std::string>();
    int beg_date = (date_beg > date_end) ? date_end : date_beg;
    int end_date = (date_beg > date_end) ? date_beg : date_end;

    auto pArg = PyTuple_New(4);
    PyTuple_SetItem(pArg, 0, Py_BuildValue("s", code.c_str()));
    char beg_date_str[32] = {0};
    sprintf_s(beg_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(beg_date), GET_LONGDATE_MONTH(beg_date), GET_LONGDATE_DAY(beg_date));
    PyTuple_SetItem(pArg, 1, Py_BuildValue("s", beg_date_str));
    char end_date_str[32] = {0};
    sprintf_s(end_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(end_date), GET_LONGDATE_MONTH(end_date), GET_LONGDATE_DAY(end_date));
    PyTuple_SetItem(pArg, 2, Py_BuildValue("s", end_date_str));

    PyTuple_SetItem(pArg, 3, Py_BuildValue("b", is_index));

    std::vector<std::string> ret_vector;
    char *result;
    try
    {
        auto pRet = PyEval_CallObject((PyObject*)PyFuncGetHourKbar2File, pArg);
        if( !pRet )
        {
            std::cout << " PyFuncGetHourKbar2File  ret null! beg_date:" << beg_date << " end_date:" << end_date << std::endl;
            local_logger().LogLocal(utility::FormatStr("PyFuncGetHourKbar2File   ret null! beg_date:%d end_date:%d", beg_date, end_date));
            return ret_vector;
        }
        PyArg_Parse(pRet, "s", &result);

        if( result && strlen(result) > 0 )
        {
            if( strstr(result, ";") )
            {
                ret_vector = utility::split(result, ";"); 
            }else
                ret_vector.push_back(result);
            Py_XDECREF(result);
        } 
    }catch(...)
    {
        Py_XDECREF(result);
    } 
    //-------------------
    return ret_vector; 
}

bool QuotationServerApp::GetRealTimeK(const std::string &code, QuotationMessage::QuotationKbarMessage &kbar_msg, bool is_index)
{
    assert(PyFuncGetRealTimeKbar);
      
    auto pArg = PyTuple_New(2);
    PyTuple_SetItem(pArg, 0, Py_BuildValue("s", code.c_str()));
    PyTuple_SetItem(pArg, 1, Py_BuildValue("b", is_index));

    //std::vector<std::string> ret_vector;
    char *result;
    try
    {
    auto pRet = PyEval_CallObject((PyObject*)PyFuncGetRealTimeKbar, pArg);
    if( !pRet )
    {
        std::cout << " GetRealTimeK  PyFuncGetRealTimeKbar " << code << " ret null! " << std::endl;
        local_logger().LogLocal(utility::FormatStr("GetRealTimeK  PyFuncGetRealTimeKbar %s ret null! ", code.c_str()));
        return false;
    }
    PyArg_Parse(pRet, "s", &result);
    
    if( result && strlen(result) > 0 )
    {
        if( strstr(result, ";") )
        {
            auto ret_vector = utility::split(result, ";"); 
            if( ret_vector.size() != 6 )
               return false;
            std::string open_str = ret_vector[2];
            std::string close_str = ret_vector[0]; // cur_price 
            std::string high_str = ret_vector[3];
            std::string low_str = ret_vector[4];
            std::string vol_str = ret_vector[5];
                 
            try
            { 
                kbar_msg.set_yyyymmdd(TSystem::Today());
                TSystem::FillRational(open_str, *kbar_msg.mutable_open()); 
                TSystem::FillRational(close_str, *kbar_msg.mutable_close());
                TSystem::FillRational(high_str, *kbar_msg.mutable_high());
                TSystem::FillRational(low_str, *kbar_msg.mutable_low());
                TSystem::FillRational(vol_str, *kbar_msg.mutable_vol());
            }catch(std::exception &e)
            {
                printf("exception:%s", e.what());
                return false;
            }catch(...)
            {
                return false;
            }
        }
        Py_XDECREF(result);
        return true;
    } 
    }catch(...)
    {
        Py_XDECREF(result);
    } 
    //-------------------
    return false; 
}


bool IsLeapYear(int year)
{
    return ( (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 );
}
 
int DaysOneMonth(int year, int month)   //返回一个月的天数 
{
    int re = 0;
    bool leap = IsLeapYear(year);    //是否是润年
    switch(month)
    {
        case 1:     //1,3,5,7,8,10,12月每月都是31天
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            re=31;
            break;
        case 4:      //4,6,9,11月每月都是30天
        case 6:
        case 9:
        case 11:
            re=30;
            break;
        case 2:
            if( leap )   
                re=29;  //是润年,则是29天
            else
                re=28;  //不是润年,则是28天
            break;
        default:
            //printf("\nmonth is error!");
            break;
    }
    return re;
}

bool IsLongDate(int date)
{ 
    //return date > 19000101 && date < 30000101;
    return date > 19000101 && GET_LONGDATE_DAY(date) <= DaysOneMonth( GET_LONGDATE_YEAR(date), GET_LONGDATE_MONTH(date) );
}


bool IsStockCode(const std::string &code)
{
    //code.find(" ") 
    //boost::lexical_cast<int>(code
    if( code.length() != 6 )
        return false;
    try
    {
        auto val = std::stoi(code);
        return (code[0] == '0' || code[0] == '3' || code[0] == '6' || code[0] == '8' || code[0] == '9');
    }catch(std::exception&)
    {
        return false;
    }
    return false;
}

// lambda to convert time
void ConvertTime(const Time& t, int& longdate, std::string * timestamp)
{
    TimePoint tp( MakeTimePoint(t.time_value(), t.frac_sec()) );
    longdate = ToLongdate( tp.year(), tp.month(), tp.day() );
    if( timestamp )
        *timestamp = utility::FormatStr("%02d:%02d:%02d.%06d", tp.hour(), tp.minute(), tp.sec(), static_cast<int>(tp.frac_sec()*1000000));
};

// ps: make sure date_end >= date_begin
std::vector<int> GetSpanTradeDates(int date_begin, int date_end)
{
    assert(date_begin <= date_end);

    static auto get_need_dates = [](int date_begin, int date_end, int y, int m, int d, std::vector<int> &out_days)
    { 
        int local_date = y * 10000 + m * 100 + d;
        if( IsLongDate(local_date) /*&& local_date >= date_begin && local_date <= date_end*/ )
            out_days.push_back(local_date);
    };

    std::vector<int> ret_days;

    const int year_begin = date_begin / 10000;
    int mon_begin = (date_begin % 10000) / 100;
    int day_begin = (date_begin % 100);
    const int year_end = date_end / 10000;
    const int mon_end = (date_end % 10000) / 100;
    const int day_end = (date_end % 100);
      
    std::array<int, 12> legal_mon = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::array<int, 31> legal_day = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10
                                     , 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
                                     , 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
     
    int y = year_begin;

    /*if( year_begin == year_end )
    {
        if( mon_begin == mon_end )
        {
            for( int d = day_begin; d <= day_end; ++d )
            {
                get_need_dates(date_begin, date_end, y, m, d, ret_days);
            }
        }
        int m_val = mon_begin;
        
        for( ; m_val < mon_end; ++m_val )
        {

        }
    }*/
    for( ; y <= year_end; ++y )
    {
        for(auto m : legal_mon)
        {
            
            for( auto d : legal_day )
            {
#if 1
                int local_date = y * 10000 + m * 100 + d;
                if( IsLongDate(local_date) && local_date >= date_begin && local_date <= date_end )
                    ret_days.push_back(local_date);
#else
                    get_need_dates(date_begin, date_end, y, m, d, ret_days);
#endif
            } 
        }
    }
    return ret_days;
}


int bar_daystr_to_longday(const std::string &day_str)
{ 
    int year, mon, day;
    std::string partten_string = "^(\\d{4})-(\\d{1,2})-(\\d{1,2})$"; 
    std::regex regex_obj(partten_string); 
    std::smatch result; 
    if( std::regex_match(day_str.cbegin(), day_str.cend(), result, regex_obj) )
    {
        try
        {
            year = boost::lexical_cast<int>(result[1]);
            mon = boost::lexical_cast<int>(result[2]);
            day = boost::lexical_cast<int>(result[3]);
        }catch(boost::exception&)
        {
            return 0;
        }
        //std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
    }
    return ToLongdate(year, mon, day);
}

void Delay(unsigned short mseconds)
{
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}