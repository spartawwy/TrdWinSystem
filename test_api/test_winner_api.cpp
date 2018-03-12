
#include <iostream>

#include <Windows.h>

#include "winner_hq_api.h"


int main()
{
    HMODULE api_handle = LoadLibrary("winner_api.dll");
    if( !api_handle )
    {
        std::cout << "LoadLibrary winner_api.dll fail" << std::endl;
        return 1;
    }
    //void *p_tchk = GetProcAddress(api_handle, "WinnerHisHq_Connect");
    WinnerHisHq_ConnectDelegate WinnerHisHq_Connect = (WinnerHisHq_ConnectDelegate)GetProcAddress(api_handle, "WinnerHisHq_Connect"); 
    if ( !WinnerHisHq_Connect )
    {
        std::cout << " GetProcAddress fail " << std::endl;
        return 1;
    }

    WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect =  (WinnerHisHq_DisconnectDelegate)GetProcAddress(api_handle, "WinnerHisHq_Disconnect"); 
    char result[1024] = {0};
    char error[1024] = {0};
    auto ret = WinnerHisHq_Connect("128.1.1.3", 50010, result, error);
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
    getchar();
    return 0;
}