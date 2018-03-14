#include "winner_client.h"

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_serialization.h>
#include <TLib/core/tsystem_time.h>

#include <TLib/tool/tsystem_connection_handshake.pb.h>

#include "WINNERLib/winner_message_system.h"
#include "WINNERLib/quotation_msg.pb.h"

using namespace TSystem;

WinnerClient::WinnerClient()
    : ServerClientAppBase("api", "winner_api", "0.1")
    , msg_handlers_(msg_system_)
    , pconn_(nullptr)
    , strand_(task_pool())
    , is_connected_(false)
    , fenbi_callback_(nullptr)
{
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
        QuotationMessage quotation_message;
        if( Decode(msg, quotation_message, this->msg_system_) )
        {
            for(int i = 0; i < quotation_message.quote_fill_msg().size(); ++i )
            {
                fenbi_callback_(nullptr, false);
            }
        }
    });
}


bool WinnerClient::ConnectServer(const char* pServerAddress, int port)
{
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

        return true;
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

bool WinnerClient::RequestFenbiHisData(char* Zqdm, int Date, FenbiCallBack call_back, char* ErrInfo)
{
    if( !is_connected_ || !pconn_ )
        return false;
    fenbi_callback_ = call_back;

    // request fenbi data from quotation server
    QuotationRequest quotation_req;
    quotation_req.set_code(Zqdm);

    FillTime(TimePoint(MakeTimePoint(2018, 3, 8)), *quotation_req.mutable_beg_time());
    FillTime(TimePoint(MakeTimePoint(2018, 3, 8)), *quotation_req.mutable_end_time());
    quotation_req.set_req_type(QuotationReqType::FENBI);

    pconn_->AsyncSend( Encode(quotation_req, msg_system_, Message::HeaderType(0, pid(), 0)));
    return true; 
}