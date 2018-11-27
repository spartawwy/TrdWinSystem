#ifndef WINNER_CLIENT_SDF3SDFSDF_H_
#define WINNER_CLIENT_SDF3SDFSDF_H_

#include <memory>
#include <atomic>
#include <unordered_map>

#include <TLib/tool/tsystem_server_client_appbase.h>
#include "boost_mutex.h"

#include "winner_his_hq.h"
//using namespace TSystem;

class WinnerClient : public TSystem::ServerClientAppBase
{
public:

    WinnerClient();
    virtual ~WinnerClient();
     
    bool ConnectServer(const char* pServerAddress, int port);
    void DisConnectServer();
    void Shutdown();

    bool is_connected() const { return is_connected_; }

    bool RequestFenbiHisData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo);
    bool RequestFenbiHisDataBatch(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char* ErrInfo);
    bool RequestKData(char* Zqdm, PeriodType type, int date_begin, int date_end
                    , T_KDataCallBack *call_back_para, bool is_index, char* ErrInfo);
    bool RequestHisQuote(char* Zqdm, int date, char* ErrInfo);

private:

    void Initiate();
     
    virtual void HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override {}
    virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override {}

    void SetupMsgHandlers();
/*
    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override;
    virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
        , const TError& te) override {};

    virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
    virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;

    virtual void UpdateState() override;*/

private:

    TSystem::MessageHandlerGroup	msg_handlers_;

    std::shared_ptr<TSystem::communication::Connection>  pconn_;

    TSystem::TaskStrand  strand_;

    std::string server_addr;
    bool is_connected_;
    
    //T_FenbiCallBack *call_back_para_; 
    //T_KDataCallBack *kdata_call_back_para_;

    std::atomic_uint request_id_;
    enum class ReqType :  unsigned char
    {
        FILL_FENBI,
        KDATA,
        HIS_QUOTE,
    };
    // (req_id, call back para)
    std::unordered_map<int, std::tuple<ReqType, void*> >  request_holder_;
    //BoostReadWriteMutex request_holder_mutex_;
};
#endif // WINNER_CLIENT_SDF3SDFSDF_H_