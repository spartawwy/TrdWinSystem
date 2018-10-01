#include "winner_his_hq.h"

#include <thread>
#include <windows.h>

#include <TLib/core/tsystem_core_paths.h>
#include "winner_client.h"
 
static WinnerClient* GetInstance(bool is_del = false);
  
extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo)
{ 
    auto ret = GetInstance()->ConnectServer(IP, Port);
    return ret ? 0 : -1;
}
 
extern "C" DLLIMEXPORT void __cdecl WinnerHisHq_Disconnect()
{
    GetInstance()->DisConnectServer();
    GetInstance(true); 
}


extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo)
{
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestFenbiHisData(Zqdm, Date, call_back_para, ErrInfo);
    return val ? 0 : -2;
}

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiDataBatch(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char* ErrInfo)
{
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestFenbiHisDataBatch(Zqdm, date_begin, date_end, call_back_para, ErrInfo);
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
            //winn_client_obj->FireShutdown(); 
            winn_client_obj->Shutdown(); 
        }
    }else if( !winn_client_obj )
    {
#if 0 
        std::thread man_thread([]()
        {
            winn_client_obj = new WinnerClient;
            //while( !winn_client_obj->HasShutdown() )  // can't get Shutdown why ?
            while( !man_thread_exit_flag )
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            delete winn_client_obj; 
            winn_client_obj = nullptr;
        });
#else 
        std::thread man_thread([]()
        {
            winn_client_obj = new WinnerClient;
            winn_client_obj->WaitShutdown();

            delete winn_client_obj; 
            winn_client_obj = nullptr;
        });
#endif
        man_thread.detach();
    }

    while( !is_del && !winn_client_obj )
        Sleep(10);
    return winn_client_obj;
}
