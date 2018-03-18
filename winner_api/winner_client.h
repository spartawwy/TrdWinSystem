#ifndef WINNER_CLIENT_SDF3SDFSDF_H_
#define WINNER_CLIENT_SDF3SDFSDF_H_

#include <memory>

#include <TLib/tool/tsystem_server_client_appbase.h>

#include "winner_his_hq.h"
//using namespace TSystem;

class WinnerClient : public TSystem::ServerClientAppBase
{
public:

    WinnerClient();
    virtual ~WinnerClient();
     
    bool ConnectServer(const char* pServerAddress, int port);
    void DisConnectServer();
    //void ShutdownAPI();   

    bool is_connected() const { return is_connected_; }

    bool RequestFenbiHisData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo);

private:

    void Initiate();

    void Shutdown();

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

    bool is_connected_;
    
    T_FenbiCallBack *call_back_para_; 
};
#endif // WINNER_CLIENT_SDF3SDFSDF_H_