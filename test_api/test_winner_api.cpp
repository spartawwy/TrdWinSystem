
#include <iostream>
#include <string>

#include <stdlib.h>
#include <vector>

#include <Windows.h>

#include <TLib/core/tsystem_utility_functions.h>

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
#if 1
    auto ret = WinnerHisHq_Connect("192.168.1.5", 50010, result, error);
#else
    auto ret = WinnerHisHq_Connect("128.1.1.3", 50010, result, error);
#endif 
    std::string cmd;
	std::vector<std::string> args;

    bool exit = false;
	while( !exit )
	{

		std::getline(std::cin, cmd);
		std::cout << cmd << std::endl;
		args = TSystem::utility::split(cmd);
        if( args.size() > 0 && args[0] == "EOF" )
        {
            break;
        }else if( args[0] == "FENBI" && args.size() > 3 ) // FENBI  stock start_date end_date  --yyyymmdd
        {

        }

    }

#if 1
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
#endif
    //getchar();
    return 0;
}