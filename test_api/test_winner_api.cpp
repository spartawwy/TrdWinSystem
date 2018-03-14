
#include <iostream>
#include <string>

#include <stdlib.h>
#include <vector>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include <Windows.h>

#include "winner_hq_api.h"

#define  TEST_OHTER

#ifdef TEST_OHTER
using namespace TSystem;
#endif

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end);

int main()
{
#ifdef TEST_OHTER
    static auto bar_daystr_to_longday = [](const std::string &day_str)->int
    {
        auto tmp_str = day_str;
        TSystem::utility::replace_all( *const_cast<std::string*>(&tmp_str), "-", ""); 
        try
        {
            std::stoi(tmp_str);
        }catch(std::exception &e)
        {
            std::cout << "bar_daystr_to_longday error: '" << day_str << "' " << e.what() << std::endl;
            return 0;
        }catch(...)
        {
            std::cout <<"bar_daystr_to_longday error: '" << day_str << "' "  << std::endl;
            return 0;
        }

        assert(day_str.size() == 10 ); // strlen("yyyy-mm-dd") == 10
        int year, mon, day;
        sscanf_s(day_str.c_str(), "%04d-%02d-%02d", &year, &mon, &day);
        return ToLongdate(year, mon, day);
         
    };
    std::string src = "2018-01-02";
    int dy = bar_daystr_to_longday(src);
    auto std_tp = TSystem::MakeTimePoint(2018, 3, 8);
    auto tp = TSystem::TimePoint(MakeTimePoint(2018, 3, 8));
    tp = tp;
    Time  t_ime;
    TSystem::FillTime(tp, t_ime);
    auto time_str = TSystem::ToString(t_ime);
    auto ckday = TSystem::Today();
    ckday = ckday;
#endif

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
        std::cout << " GetProcAddress WinnerHisHq_Connect fail " << std::endl;
        return 1;
    }

    WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect =  (WinnerHisHq_DisconnectDelegate)GetProcAddress(api_handle, "WinnerHisHq_Disconnect"); 
    
    WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData 
        = (WinnerHisHq_GetHisFenbiDataDelegate)GetProcAddress(api_handle, "WinnerHisHq_GetHisFenbiData"); 
    if( !WinnerHisHq_GetHisFenbiData )
    {
        std::cout << " GetProcAddress WinnerHisHq_GetHisFenbiData fail " << std::endl;
        return 1;
    }
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
        if( args.size() <= 0 )
            continue;
        if( args[0] == "EOF" )
        {
            break;
        }else if( args[0] == "FENBI" && args.size() > 3 ) // FENBI  stock start_date end_date  --yyyymmdd
        {
            auto val_ret = WinnerHisHq_GetHisFenbiData("600487", 20080308, FenbiCallBackFun, error);
            if( val_ret != 0 )
                std::cout << " WinnerHisHq_GetHisFenbiData fail !" << std::endl;
        }

    }

#if 1
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
#endif
    //getchar();
    return 0;
}

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end)
{
    if( quote_data )
    {
        quote_data->code;
    }
    if( is_end )
        is_end = is_end;
}