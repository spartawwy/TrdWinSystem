#include "winner_his_hq.h"

#include "winner_client.h"

#include <TLib/core/tsystem_core_paths.h>

static WinnerClient* GetInstance(bool is_del = false);
 
extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo)
{
    
    auto ret = GetInstance()->ConnectServer(IP, Port);

    return ret ? 0 : -1;
}

extern "C" DLLIMEXPORT void __cdecl WinnerHisHq_Disconnect()
{
    GetInstance(true); 
}

static WinnerClient* GetInstance(bool is_del)
{
    static std::string pro_tag = TSystem::utility::ProjectTag("wzf");
    static WinnerClient * winn_client_obj = new WinnerClient;
    if( is_del )
    {
        if( winn_client_obj ) 
        {
            winn_client_obj->FireShutdown();
            delete winn_client_obj; 
            winn_client_obj = nullptr;
        }
    }else if( !winn_client_obj )
    {
        winn_client_obj = new WinnerClient;
    }

    return winn_client_obj;
}
