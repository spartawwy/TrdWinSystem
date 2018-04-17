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

#include "file_mapping.h"

#ifdef _DEBUG
#undef Py_XDECREF
#define Py_XDECREF(a) (a)
#endif
//#pragma comment(lib, "python36.lib")
#define  GET_LONGDATE_YEAR(a) (assert(a > 10000000), a/10000)
#define  GET_LONGDATE_MONTH(a) (assert(a > 10000000), (a%10000)/100)
#define  GET_LONGDATE_DAY(a) (assert(a > 10000000), (a%100))

static bool IsLongDate(int date);
static bool IsStockCode(const std::string &code);
static void ConvertTime(const Time& t, int& longdate, std::string * timestamp=nullptr);
static std::vector<int> GetSpanTradeDates(int date_begin, int date_end);

static bool IsLeapYear(int year);
static int DaysOneMonth(int year, int month);

QuotationServerApp::QuotationServerApp(const std::string &name, const std::string &version)
	: ServerAppBase("quotation_server", name, version)
    , PyFuncGetAllFill2File(nullptr)
    , stk_data_dir_()
    , conn_strands_(256)
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
    stk_data_dir_ = stk_data_dir;
    if( stk_data_dir_.rfind("\\") != stk_data_dir_.length() - 1 && stk_data_dir_.rfind("/") != stk_data_dir_.length() - 1 )
    {
       stk_data_dir_ += "/";
    }
	/*db_moudle_.Init();

	CreateStockTickers();*/
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

    //导入模块   
    PyObject* pModule = PyImport_ImportModule("get_stk_his_fill2file"); // get_stock_his.py
    if (!pModule)  
    {  
        std::cout << "QuotationServerApp::InitPython PyImport_ImportModule get_stk_his_fill2file fail " << std::endl;
		// todo: throw exception
		throw ""; 
    }  
    PyObject *pDic = PyModule_GetDict(pModule);
    if (!pDic)  
    {  
        std::cout << "QuotationServerApp::InitPython PyModule_GetDict of get_stk_his_fill2file fail " << std::endl;
		// todo: throw exception
		throw ""; 
    }  

    PyObject* pcls = PyObject_GetAttrString(pModule, "STOCK");   
    if (!pcls || !PyCallable_Check(pcls))  
    {  
        std::cout << "QuotationServerApp::InitPython PyObject_GetAttrString class STOCK fail " << std::endl;
		// todo: throw exception
		throw ""; 
    }  
 
    PyObject* p_stock_obj = PyEval_CallObject(pcls, NULL);
    if( !p_stock_obj )
    { 
        std::cout << "QuotationServerApp::InitPython PyEval_CallObject create STOCK obj fail " << std::endl;
		// todo: throw exception
		throw ""; 
    }
    PyFuncGetAllFill2File = PyObject_GetAttrString(p_stock_obj, "getAllFill2File");
    if( !PyFuncGetAllFill2File )
    {
        std::cout << "QuotationServerApp::InitPython get func getAllFill2File fail " << std::endl;
		// todo: throw exception
		throw ""; 
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
	SendRequestAck(req.user_id(), req.request_id(), req.request_type(), pconn);
}

void QuotationServerApp::HandleQuotationRequest(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn)
{
   assert(req);
   assert(pconn); 
   static auto time_to_longday = [](const Time& t_m) ->int
   {
        auto tm_point = TSystem::MakeTimePoint((time_t)t_m.time_value(), t_m.frac_sec());
        TimePoint t_p(tm_point);
        return ToLongdate(t_p.year(), t_p.month(), t_p.day());
   };
   static auto bar_daystr_to_longday = [](const std::string &day_str)->int
   { 
       auto tmp_str = day_str;
       TSystem::utility::replace_all( *const_cast<std::string*>(&tmp_str), "-", ""); 
       try
       {
           std::stoi(tmp_str);
       }catch(std::exception &e)
       {
           std::cout << "bar_daystr_to_longday error: '" << day_str << "' " << e.what() << std::endl;
           return 0;
       }catch(...)
       {
           std::cout <<"bar_daystr_to_longday error: '" << day_str << "' "  << std::endl;
           return 0;
       }
        
       assert(day_str.size() == 10 ); // strlen("yyyy-mm-dd") == 10
       int year, mon, day;
       sscanf_s(day_str.c_str(), "%04d-%02d-%02d", &year, &mon, &day);
       return ToLongdate(year, mon, day);
   };
     
    //printf("\n to GetFenbi2File\n");
    //// format: yyyy-mm-dd
    //auto data_str_vector = GetFenbi2File(req->code(), time_to_longday(req->beg_time()), time_to_longday(req->end_time()));
    //printf("\n ret GetFenbi2File\n");
    
    static auto fetch_data_send 
        = [this](std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn, int begin_date, int end_date)
    {
        auto ret_date_str_vector = GetFenbi2File(req->code(), begin_date, end_date);
        std::for_each( std::begin(ret_date_str_vector), std::end(ret_date_str_vector), [&req, &pconn, this](const std::string &entry)
        { 
            /*auto date_comp = TSystem::FromLongdate(longdate);
            char bar_date_str[64] = {0};
            sprintf_s(bar_date_str, sizeof(bar_date_str), "%04d-%02d-%02d", std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)); 
           */
            std::string full_path = this->stk_data_dir_ + req->code() + "/" + entry + "/fenbi/" + req->code() + ".fenbi";
            FileMapping file_map_obj;
            if( !file_map_obj.Create(full_path) )
            {
                std::cout << "map " << full_path << " fail!\n";
                return;
            }
            char *p0 = file_map_obj.data_address(); 
            // ps: every file is start with 10000 
            if( file_map_obj.file_size() < 150 || !isdigit(*(p0 + 6)) )
            {
                std::cout << "warning: illegal content in " << full_path << std::endl;
                return;
            }
            QuotationMessage quotation_msg;
            quotation_msg.set_code(req->code());

            //std::string id;
            std::string hour;
            std::string minute;
            std::string second;
            std::string price;
            std::string change_price;
            std::string vol;
            //std::string amount; 
        
            char *p1 = p0; 
            char strbuf[1024] = {0};
            while( *p1 != '\0' ) 
            {
                int count = 0;
                p0 = p1;
                while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
                if( *p1 == '\0' ) break; 
                // id relate  ------
    #if 0  //nouse
                if( count > 0 )
                {  
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0'; 
                    id = strbuf; 
                }
    #endif
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
    #if 0 // nouse
                if( count > 0 )
                { 
                    memcpy(strbuf, p0, count);
                    strbuf[count] = '\0'; 
                    amount = strbuf;
                } 
    #endif
                // filter 0x0A
                if( *p1 != '\0' )
                    ++p1;
    #if 1
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
#endif
                //std::cout <<  price << " " << change_price << " " << vol << std::endl;
            } // while 

            printf("pconn->AsyncSend\n");
            pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) ); 

        }); // std::for_each file

    }; // fetch_data_send

    auto tm_point = TSystem::MakeTimePoint((time_t)req->beg_time().time_value(), req->beg_time().frac_sec());
    auto tm_point_end = TSystem::MakeTimePoint((time_t)req->end_time().time_value(), req->end_time().frac_sec());
 
    TimePoint t_p(tm_point);
    int longday_beg = ToLongdate(t_p.year(), t_p.month(), t_p.day());
    
    TimePoint t_p_end(tm_point_end);
    int longday_end = ToLongdate(t_p_end.year(), t_p_end.month(), t_p_end.day());

    auto date_vector = GetSpanTradeDates(longday_beg, longday_end);

    const int span_len = 5;
    int i = 0;
    for( ; i < date_vector.size() / span_len; ++i )
    { 
       fetch_data_send(req, pconn, date_vector[i*span_len], date_vector[(i + 1)*span_len - 1]);
       if( i < 10 )
           Delay(20);
       else if(i < 20 )
           Delay(40);
       else                                                                                                                                                                                                                                                                                                                                                                                                                                                          
           Delay(80); 
    }
     
    if( date_vector.size() % span_len  )
    {
        fetch_data_send(req, pconn, date_vector[i*span_len], date_vector[date_vector.size() - 1]);
    }
}


void QuotationServerApp::SendRequestAck(int user_id, int req_id, RequestType type, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	UserRequestAck ack;
	ack.set_req_type(type);
	ack.set_user_id(user_id);
	TSystem::FillRequestAck(req_id, *ack.mutable_req_ack());
	pconn->AsyncSend( Encode(ack, msg_system(), Message::HeaderType(0, pid(), 0)));
}

std::vector<std::string> QuotationServerApp::GetFenbi2File(const std::string &code, int date_beg, int date_end)
{
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
        *timestamp = utility::FormatStr("%02d:%02d:%02d.%06d", tp.hour(), tp.min(), tp.sec(), static_cast<int>(tp.frac_sec()*1000000));
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

void Delay(unsigned short mseconds)
{
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}