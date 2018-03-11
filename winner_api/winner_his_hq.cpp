#include "winner_his_hq.h"

#include "winner_client.h"

static WinnerClient& GetInstance()
{
    static WinnerClient winn_client_obj;
    return winn_client_obj;
}

extern "C" DllExport bool __stdcall WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo)
{
    auto ret = GetInstance().ConnectServer(IP, Port);

    return ret;
}

extern "C" DllExport void __stdcall WinnerHisHq_Disconnect()
{
    GetInstance().ShutdownAPI();
}