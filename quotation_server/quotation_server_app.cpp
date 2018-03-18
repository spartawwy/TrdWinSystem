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

QuotationServerApp::QuotationServerApp(const std::string &name, const std::string &version)
	: ServerAppBase("quotation_server", name, version)
    , PyFuncGetAllFill2File(nullptr)
    , stk_data_dir_()
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
            this->task_pool_.PostTask( std::bind(&QuotationServerApp::HandleQuotationRequest, this, std::move(req), p->shared_this()));
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
   QuotationMessage quotation_msg;
   quotation_msg.set_code(req->code());

    auto tm_point = TSystem::MakeTimePoint((time_t)req->beg_time().time_value(), req->beg_time().frac_sec());
    TimePoint t_p(tm_point);
    int longday_beg = ToLongdate(t_p.year(), t_p.month(), t_p.day());

    // format: yyyy-mm-dd
    auto data_str_vector = GetFenbi2File(req->code(), time_to_longday(req->beg_time()), time_to_longday(req->end_time()));
#if 1 
    std::for_each( std::begin(data_str_vector), std::end(data_str_vector), [&req, &quotation_msg, this](std::string & entry)
    { 
        int longdate = bar_daystr_to_longday(entry);
        if( longdate == 0 )
        {
            return;
        }
        std::string full_path = this->stk_data_dir_ + req->code() + "/" + entry + "/fenbi/" + req->code() + ".fenbi";
        FileMapping file_map_obj;
        if( !file_map_obj.Create(full_path) )
        {
            std::cout << "map " << full_path << " fail!";
            return;
        }
        //std::string id;
        std::string hour;
        std::string minute;
        std::string second;
        std::string price;
        std::string change_price;
        std::string vol;
        //std::string amount; 
        char *p0 = file_map_obj.data_address(); 
        char *p1 = p0; 
        char strbuf[1024] = {0};
        while( *p1 != '\0' ) 
        {
            int count = 0;
            p0 = p1;
            while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
            if( *p1 == '\0' ) break; 
            // id relate  ------
#if 0
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
#if 0
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

            QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();

            auto date_comp = TSystem::FromLongdate(longdate);
            FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                , std::stoi(hour), std::stoi(minute), std::stoi(second)))
                , *p_fill_msg->mutable_time() );
            TSystem::FillRational(price, *p_fill_msg->mutable_price()); 
            TSystem::FillRational(change_price, *p_fill_msg->mutable_price_change());
            if( std::stod(change_price) < 0.0 )
                p_fill_msg->set_is_change_positive(false);

            p_fill_msg->set_vol(std::stoi(vol));
            //std::cout <<  price << " " << change_price << " " << vol << std::endl;
        } // while 
          
    }); // std::for_each file
#endif
    pconn->AsyncSend( Encode(quotation_msg, msg_system(), Message::HeaderType(0, pid(), 0)) );
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
    sprintf_s(end_date_str, "%d-%02d-%02d\0", GET_LONGDATE_YEAR(beg_date), GET_LONGDATE_MONTH(beg_date), GET_LONGDATE_DAY(beg_date));
    PyTuple_SetItem(pArg, 2, Py_BuildValue("s", end_date_str));

    auto pRet = PyEval_CallObject((PyObject*)PyFuncGetAllFill2File, pArg);
    char *result;
    PyArg_Parse(pRet, "s", &result);
    std::vector<std::string> ret_vector;
    if( result && strlen(result) > 0 )
    {
        if( strstr(result, ";") )
        {
            ret_vector = utility::split(result, ";"); 
        }else
            ret_vector.push_back(result);
    } 
    Py_XDECREF(result);
    //-------------------
    return ret_vector; 
}

bool IsLongDate(int date)
{
    /*if( date < 10000000 && date > 30000000 )
        return false;*/
    return date > 19000101 && date < 30000101;
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