#include "winner_client.h"

#include <array>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_time.h>

#include <TLib/tool/tsystem_connection_handshake.pb.h>
#include <TLib/tool/tsystem_rational_number.h>

#include "WINNERLib/winner_message_system.h"
#include "WINNERLib/quotation_msg.pb.h"

bool IsLongDate(int date);

using namespace TSystem;

WinnerClient::WinnerClient()
    : ServerClientAppBase("api", "winner_api", "0.1")
    , msg_handlers_(msg_system_)
    , pconn_(nullptr)
    , strand_(task_pool())
    , is_connected_(false)
   /* , call_back_para_(nullptr)
    , kdata_call_back_para_(nullptr)*/
    , request_holder_(1024)
{
    request_id_ = 0;
    try
    {
        Initiate();
    }catch(const TSystem::TException& e)
    {
        LogError(LogMessage::VITAL, e.error(), local_logger());
    }
}

WinnerClient::~WinnerClient()
{
    /*FireShutdown();*/
}

void WinnerClient::Shutdown()
{
	ServerClientAppBase::Shutdown();
}

//void WinnerClient::ShutdownAPI()
//{
//    try
//    {
//        Shutdown();
//    }catch(const TSystem::TException& e)
//    {
//        LogError(LogMessage::VITAL, e.error(), local_logger());
//    }
//}


void WinnerClient::Initiate()
{
    std::string cur_dir(".//");
    work_dir(cur_dir);
    local_logger_.SetDir(cur_dir);
    local_logger_.SetName(name());

    task_pool_.AddWorker();
    local_logger_.StartFlush();

    WINNERSystem::LoadClassID(msg_system_);

    this->pid(50000); // hard code for pid
    SetupMsgHandlers();
}

void WinnerClient::SetupMsgHandlers()
{
    msg_handlers_.RegisterHandler("HandShake", [this](communication::Connection* p, const Message& msg)
	{
		this->local_logger().LogLocal( utility::FormatStr("receive handshake from connection: %d", p->connid()));
		HandShake hs;
		if( Decode(msg, hs, this->msg_system_) )
		{
			p->hand_shaked(true);
			//this->callbacks_.OnConnectServer(true);
			this->pconn_ = p->shared_this();
            is_connected_ = true;
		}
	});

    msg_handlers_.RegisterHandler("QuotationMessage", [this](communication::Connection* p, const Message& msg)
    {
        this->local_logger().LogLocal( utility::FormatStr("receive QuotationMessage from connection: %d", p->connid()));
        auto quotation_message = std::make_shared<QuotationMessage>();
        if( !Decode(msg, *quotation_message, this->msg_system_) )
            return;
        strand_.PostTask([quotation_message, this]()
        {  
            //auto read_locker = request_holder_mutex_.ReadLock();
            auto req_iter = request_holder_.find(quotation_message->req_id());
            if( req_iter == request_holder_.end() )
            {   // log error
                    return;
            }
            void *call_back_para = nullptr;
            void *tmp_ptc = nullptr;
            if( std::get<0>(req_iter->second) == ReqType::FILL_FENBI )
            {
                call_back_para = static_cast<T_FenbiCallBack *>(std::get<1>(req_iter->second));
                tmp_ptc = ((T_FenbiCallBack*)call_back_para)->call_back_func;
                if( !call_back_para || !tmp_ptc )
                    return;
                for(int i = 0; i < quotation_message->quote_fill_msgs().size(); ++i )
                {
                    T_QuoteAtomData quote_atom_data = {0};
                     
                    QuotationMessage::QuotationFillMessage *msg_fill = quotation_message->mutable_quote_fill_msgs()->Mutable(i);
 
                    strcpy_s(quote_atom_data.code, quotation_message->code().c_str());

                    quote_atom_data.time = msg_fill->time().time_value();
                        
                    quote_atom_data.price = RationalDouble(msg_fill->price());
                    quote_atom_data.price_change = RationalDouble(msg_fill->price_change()) * (msg_fill->is_change_positive() ? 1 : -1);
                    
                    quote_atom_data.vol = msg_fill->vol();
                      
                    ((T_FenbiCallBack*)call_back_para)->call_back_func(&quote_atom_data, quotation_message->is_last(), call_back_para);
                    if( quotation_message->is_last() )
                    {
                        request_holder_.erase(req_iter);
                        return;
                    }
                }
            }else if( std::get<0>(req_iter->second) == ReqType::HIS_QUOTE )
            {
                // todo:
                auto his_quote_iter = his_quote_container_.find(quotation_message->code());
                if( his_quote_iter == his_quote_container_.end() )
                    his_quote_iter = his_quote_container_.insert(std::make_pair(quotation_message->code(), TIntMapTimeQuoteData())).first;
                if( quotation_message->quote_fill_msgs().size() <= 0 )
                {
                    request_holder_.erase(req_iter);
                    return;
                }
                auto time_val = quotation_message->quote_fill_msgs().Get(0).time().time_value();
                struct tm timeinfo;
                localtime_s(&timeinfo, &time_val); 
                const int long_date = (timeinfo.tm_year + 1900) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;
                // already has related data
                if( his_quote_iter->second.find(long_date) != his_quote_iter->second.end() )
                    return;

                auto date_quotes = his_quote_iter->second.insert( std::make_pair(long_date, TTimeMapQuoteData()) ).first;
                for(int i = 0; i < quotation_message->quote_fill_msgs().size(); ++i )
                {
                    auto p_quote_atom_data = std::make_shared<QuoteAtomData>();
                    T_QuoteAtomData &quote_atom_data = p_quote_atom_data->data_;
                    memset(&quote_atom_data, 0, sizeof(quote_atom_data));
                    //his_quote_iter->second.insert(
                    QuotationMessage::QuotationFillMessage *msg_fill = quotation_message->mutable_quote_fill_msgs()->Mutable(i);
                    
                    strcpy_s(quote_atom_data.code, quotation_message->code().c_str());

                    quote_atom_data.time = msg_fill->time().time_value();
                    quote_atom_data.price = RationalDouble(msg_fill->price());
                    quote_atom_data.price_change = RationalDouble(msg_fill->price_change()) * (msg_fill->is_change_positive() ? 1 : -1);
                    quote_atom_data.vol = msg_fill->vol();
                    quote_atom_data.b_1 = RationalDouble(msg_fill->b_1());
                    quote_atom_data.b_2 = RationalDouble(msg_fill->b_2());
                    quote_atom_data.b_3 = RationalDouble(msg_fill->b_3());
                    quote_atom_data.b_4 = RationalDouble(msg_fill->b_4());
                    quote_atom_data.b_5 = RationalDouble(msg_fill->b_5());
                    quote_atom_data.s_1 = RationalDouble(msg_fill->s_1());
                    quote_atom_data.s_2 = RationalDouble(msg_fill->s_2());
                    quote_atom_data.s_3 = RationalDouble(msg_fill->s_3());
                    quote_atom_data.s_4 = RationalDouble(msg_fill->s_4());
                    quote_atom_data.s_5 = RationalDouble(msg_fill->s_5());
                    {
                        his_quote_container_mutex_.WriteLock();
                        date_quotes->second.insert( std::make_pair(quotation_message->quote_fill_msgs().Get(0).time().time_value()
                            , std::move(p_quote_atom_data)) );
                    }
                    
                    if( quotation_message->is_last() )
                    {
                        request_holder_.erase(req_iter);
                        return;
                    }
                }// for all fills

            }else if( std::get<0>(req_iter->second) == ReqType::KDATA )
            {
                call_back_para = static_cast<T_KDataCallBack *>(std::get<1>(req_iter->second));
                tmp_ptc = ((T_KDataCallBack*)call_back_para)->call_back_func;
                if( !call_back_para || !tmp_ptc )
                    return;
                for(int i = 0; i < quotation_message->kbar_msgs().size(); ++i )
                {
                    T_K_Data quote_atom_data = {0};
                     
                    QuotationMessage::QuotationKbarMessage *msg_kbar = quotation_message->mutable_kbar_msgs()->Mutable(i);
 
                    //strcpy_s(quote_atom_data.code, quotation_message->code().c_str());
                    quote_atom_data.yyyymmdd = msg_kbar->yyyymmdd();
                    if( msg_kbar->has_hhmmdd() )
                        quote_atom_data.hhmmdd = msg_kbar->hhmmdd();
                    quote_atom_data.open = RationalDouble(msg_kbar->open());
                    quote_atom_data.close = RationalDouble(msg_kbar->close());
                    quote_atom_data.high = RationalDouble(msg_kbar->high());
                    quote_atom_data.low = RationalDouble(msg_kbar->low());
                    quote_atom_data.vol = RationalDouble(msg_kbar->vol());
                    ((T_KDataCallBack*)call_back_para)->call_back_func(&quote_atom_data, i == quotation_message->kbar_msgs().size() - 1, call_back_para);
                    if( i == quotation_message->kbar_msgs().size() - 1 )
                    {
                        request_holder_.erase(req_iter);
                        return;
                    }
                }
            }
                
        }); // strand_

    }); 
}


bool WinnerClient::ConnectServer(const char* pServerAddress, int port)
{
    if( server_addr == pServerAddress && pconn_ && is_connected() )
    {
        local_logger().LogLocal(utility::FormatStr("warning : WinnerClient::ConnectServer %s already connected!", pServerAddress) );
        return true;
    }
    try
    { 
        std::string connect_str = utility::FormatStr("%s:%d", pServerAddress, port);

        comm_dock_.AsyncConnect( pServerAddress, port
            , [this, connect_str](std::shared_ptr<communication::Connection>& pconn, const TSystem::TError& te)
        {
            if( pconn && !te )
            {
                std::weak_ptr<communication::Connection> weak_pconn(pconn);
                // on connect handler
                pconn->event_handlers().on_connect = [weak_pconn, this, connect_str]()
                {
                    std::shared_ptr<communication::Connection> p = weak_pconn.lock();
                    if( p )
                    {
                        this->local_logger().LogLocal("sending handshake to server " + connect_str);
                        SendHandShake(*this, p);
                    }
                };

                // on message handler
                this->msg_handlers_.LinkConnection(pconn);

                // on disconnect handler
                pconn->event_handlers().on_disconnect = [this](const TError& te)
                {
                    if( te )
                    {
                        //this->local_logger_.LogLocal(ErrorString(te));
                        this->local_logger().LogLocal(std::string("connection drop:") + ErrorString(te));
                        // todo: this->callbacks_.OnDisconnectServer(false);
                    }else
                    {
                        this->local_logger().LogLocal("connection disconnect");
                        // todo: this->callbacks_.OnDisconnectServer(true);
                    }
                    is_connected_ = false; 
                };

                // start writing / reading loop
                pconn->Start();

            }else
            {
                // todo: this->callbacks_.OnConnectServer(false);
            }
        }
        , communication::Connection::Type::tcp);
        //this->local_logger().LogLocal("Waitfor Connection ");
        TSystem::WaitFor( [this]()->bool
        { 
            return this->is_connected_;
        }, 10 * 1000 * 1000);

        this->local_logger().LogLocal("ret Waitfor Connection ");
        return this->is_connected_;
        
    }catch(const TException& e)
    {
        LogError(LogMessage::VITAL, e.error(), local_logger());
        return false;
    }
}
 
void WinnerClient::DisConnectServer()
{
    comm_dock_.Shutdown();
    is_connected_ = false;
}

bool WinnerClient::RequestFenbiHisData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo)
{
    if( !is_connected_ || !pconn_ )
    {
        if( ErrInfo ) strcpy(ErrInfo, "server is not connected!");
        return false;
    }
    try
    {
        std::stoi(Zqdm);
    }catch(...)
    {
        if( ErrInfo ) sprintf(ErrInfo, "code:%d is illegal ", Zqdm);
        return false;
    }
    if( !IsLongDate(Date) )
    {
        if( ErrInfo ) sprintf(ErrInfo, "date:%d is illegal ", Date);
        return false;
    }
    //call_back_para_ = call_back_para;

    // request fenbi data from quotation server
    QuotationRequest quotation_req;
    quotation_req.set_code(Zqdm);

    auto date_com = FromLongdate(Date);
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_com), std::get<1>(date_com), std::get<2>(date_com))), *quotation_req.mutable_beg_time());
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_com), std::get<1>(date_com), std::get<2>(date_com))), *quotation_req.mutable_end_time());
    quotation_req.set_req_type(QuotationReqType::FENBI);
    quotation_req.set_req_id(++request_id_);
    strand_.PostTask([call_back_para, this]()
    {
         this->request_holder_.insert(std::make_pair(request_id_, std::make_tuple(ReqType::FILL_FENBI, call_back_para)));
    });
    pconn_->AsyncSend( Encode(quotation_req, msg_system_, Message::HeaderType(0, pid(), 0)));
    return true; 
}

bool WinnerClient::RequestFenbiHisDataBatch(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char* ErrInfo)
{
    if( !is_connected_ || !pconn_ )
    {
        if( ErrInfo ) strcpy(ErrInfo, "server is not connected!");
        return false;
    }
    try
    {
        std::stoi(Zqdm);
    }catch(...)
    {
        if( ErrInfo ) sprintf(ErrInfo, "code:%d is illegal ", Zqdm);
        return false;
    }
    if( !IsLongDate(date_begin) || !IsLongDate(date_end) )
    {
        if( ErrInfo ) sprintf(ErrInfo, "date:%d or %d is illegal ", date_begin, date_end);
        return false;
    }
    //call_back_para_ = call_back_para;

    // request fenbi data from quotation server
    QuotationRequest quotation_req;
    quotation_req.set_code(Zqdm);

    auto date_begin_comm = FromLongdate(date_begin);
    auto date_end_comm = FromLongdate(date_end);
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_begin_comm), std::get<1>(date_begin_comm), std::get<2>(date_begin_comm))), *quotation_req.mutable_beg_time());
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_end_comm), std::get<1>(date_end_comm), std::get<2>(date_end_comm))), *quotation_req.mutable_end_time());
    quotation_req.set_req_type(QuotationReqType::FENBI);
    quotation_req.set_req_id(++request_id_);
    strand_.PostTask([call_back_para, this]()
    {
         this->request_holder_.insert(std::make_pair(request_id_, std::make_tuple(ReqType::FILL_FENBI, call_back_para)));
    });
    pconn_->AsyncSend( Encode(quotation_req, msg_system_, Message::HeaderType(0, pid(), 0)));
    return true; 
}

bool WinnerClient::RequestKData(char* Zqdm, PeriodType type, int date_begin, int date_end
                  , T_KDataCallBack *call_back_para, bool is_index, char* ErrInfo)
{
    if( !is_connected_ || !pconn_ )
    {
        if( ErrInfo ) strcpy(ErrInfo, "server is not connected!");
        return false;
    }
    try
    {
        std::stoi(Zqdm);
    }catch(...)
    {
        if( ErrInfo ) sprintf(ErrInfo, "code:%d is illegal ", Zqdm);
        return false;
    }
    if( !IsLongDate(date_begin) || !IsLongDate(date_end) )
    {
        if( ErrInfo ) sprintf(ErrInfo, "date:%d or %d is illegal ", date_begin, date_end);
        return false;
    }
    //T_KDataCallBack * kdata_callback = call_back_para;

    // request fenbi data from quotation server
    QuotationRequest quotation_req;
    quotation_req.set_code(Zqdm);

    auto date_begin_comm = FromLongdate(date_begin);
    auto date_end_comm = FromLongdate(date_end);
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_begin_comm), std::get<1>(date_begin_comm), std::get<2>(date_begin_comm))), *quotation_req.mutable_beg_time());
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_end_comm), std::get<1>(date_end_comm), std::get<2>(date_end_comm))), *quotation_req.mutable_end_time());
    switch(type)
    {
    case PeriodType::PERIOD_15M:
        quotation_req.set_req_type(QuotationReqType::FIFTEEN_MINUTE);
        break;
    case PeriodType::PERIOD_30M:
        quotation_req.set_req_type(QuotationReqType::THIRTY_MINUTE);
        break;
    case PeriodType::PERIOD_HOUR:
        quotation_req.set_req_type(QuotationReqType::HOUR);
        break;
    case PeriodType::PERIOD_DAY:
        quotation_req.set_req_type(QuotationReqType::DAY);
        break;
    case PeriodType::PERIOD_WEEK:
        quotation_req.set_req_type(QuotationReqType::WEEK);
        break;
    default:
        return false;
    }

    QuotationFqType fq = QuotationFqType::FQ_BEFORE;
    /* switch (fq_type)
    {
    case FqType::FQ_BEFORE: fq = QuotationFqType::FQ_BEFORE;break;
    case FqType::FQ_AFTER:  fq = QuotationFqType::FQ_AFTER; break;
    case FqType::FQ_NO:     fq = QuotationFqType::FQ_NO;    break;
    }*/
    quotation_req.set_fq_type(fq);
    quotation_req.set_is_index(is_index);
    quotation_req.set_req_id(++request_id_);
     
    strand_.PostTask([call_back_para, this]()
    {
         this->request_holder_.insert(std::make_pair(request_id_, std::make_tuple(ReqType::KDATA, call_back_para)));
    });
    pconn_->AsyncSend( Encode(quotation_req, msg_system_, Message::HeaderType(0, pid(), 0)));

    return true;
}

WinnerClient::TTimeMapQuoteData * WinnerClient::FindHisQuote(const std::string &code, int date)
{
    //TTimeMapQuoteData * p_time_his_quote = nullptr;
     
    his_quote_container_mutex_.ReadLock();
    auto iter_code_his_quote = his_quote_container_.find(code);
    if( iter_code_his_quote == his_quote_container_.end() )
        return nullptr;
    auto iter_date_code_his = iter_code_his_quote->second.find(date);
    if( iter_date_code_his == iter_code_his_quote->second.end() )
        return nullptr;
    return std::addressof(iter_date_code_his->second);
}

bool WinnerClient::RequestHisQuote(char* Zqdm, int date, char* ErrInfo)
{
    TTimeMapQuoteData * p_time_his_quote = nullptr;

    if( !is_connected_ || !pconn_ )
    {
        if( ErrInfo ) strcpy(ErrInfo, "server is not connected!");
        return false;
    }
    try
    {
        std::stoi(Zqdm);
    }catch(...)
    {
        if( ErrInfo ) sprintf(ErrInfo, "code:%d is illegal ", Zqdm);
        return false;
    }
    if( !IsLongDate(date) )
    {
        if( ErrInfo ) sprintf(ErrInfo, "date:%d is illegal ", date);
        return false;
    }
    //call_back_para_ = call_back_para;

    // request fenbi data from quotation server
    QuotationRequest quotation_req;
    quotation_req.set_code(Zqdm);

    auto date_begin_comm = FromLongdate(date);
    FillTime(TimePoint(MakeTimePoint(std::get<0>(date_begin_comm), std::get<1>(date_begin_comm), std::get<2>(date_begin_comm))), *quotation_req.mutable_beg_time()); 
    quotation_req.set_req_type(QuotationReqType::DAY);
    quotation_req.set_req_id(++request_id_);
    strand_.PostTask([this]()
    {
        this->request_holder_.insert(std::make_pair(request_id_, std::make_tuple(ReqType::HIS_QUOTE, nullptr)));
    });
    pconn_->AsyncSend( Encode(quotation_req, msg_system_, Message::HeaderType(0, pid(), 0)));
      
    TSystem::WaitFor([Zqdm, date, &p_time_his_quote, this]()->bool
    {
        p_time_his_quote = FindHisQuote(Zqdm, date);
        return p_time_his_quote;
    }, 5*1000*1000); //microseconds

    return true; 
}

bool IsLongDate(int date)
{
    /*if( date < 10000000 && date > 30000000 )
        return false;*/
    return date > 19000101 && date < 30000101;
}
