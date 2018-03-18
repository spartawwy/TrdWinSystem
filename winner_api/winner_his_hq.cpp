#include "winner_his_hq.h"

#include <thread>
#include <windows.h>

#include <TLib/core/tsystem_core_paths.h>
#include "winner_client.h"
 
static WinnerClient* GetInstance(bool is_del = false);
  
extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo)
{
#if 1
    auto ret = GetInstance()->ConnectServer(IP, Port);
    return ret ? 0 : -1;
#else
     GetInstance();
     return -1;
#endif 
    
}
 
extern "C" DLLIMEXPORT void __cdecl WinnerHisHq_Disconnect()
{

#if 1 
    GetInstance(true); 
#else
    GetInstance()->DisConnectServer();
#endif
}


extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo)
{
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestFenbiHisData(Zqdm, Date, call_back_para, ErrInfo);
    return val ? 0 : -2;
}

static WinnerClient* GetInstance(bool is_del)
{
    static std::string pro_tag = TSystem::utility::ProjectTag("wzf");
    static WinnerClient * winn_client_obj = nullptr;
    if( is_del )
    {
        if( winn_client_obj ) 
        {
            winn_client_obj->FireShutdown(); 
            
        }
    }else if( !winn_client_obj )
    {
        std::thread man_thread([]()
        {
            winn_client_obj = new WinnerClient;
            winn_client_obj->WaitShutdown();

            delete winn_client_obj; 
            winn_client_obj = nullptr;
        });
        man_thread.detach();
    }

    while( !is_del && !winn_client_obj )
        Sleep(10);
    return winn_client_obj;
}
