
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

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end, void *para);
 
void testFenbiStrMatch()
{
    std::string id;
    std::string hour;
    std::string minute;
    std::string second;
    std::string price;
    std::string change_price;
    std::string vol;
    std::string amount; 

    char buf[1024] = {0};
    strcpy(buf, "13908 14:59:41 38.65 -0.01 14 54110\n13909 14:59:45 38.65 0.0 10 38650\n");

    char *p0 = buf;
    char *p1 = p0;

    char strbuf[1024] = {0};
    while( *p1 != '\0' )
    {
        int count = 0;
        p0 = p1;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        // id relate  ------
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0'; 
            id = strbuf;
        }
        // time realte -----
        p0 = ++p1;
        count = 0;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0';  
            //char *ptr1 = strbuf;
            strbuf[2] = '\0';
            hour = strbuf;

            //ptr1 = strbuf + 3;
            strbuf[5] = '\0';
            minute = strbuf + 3;

            second = strbuf + 6;

            /*char *ptr1 = strbuf;
            char temp_buf[16] = {0}; 
            char *ptr0 = ptr1;
            int num = 0;
            while( *ptr1 != ':' ) { ++ptr1; ++num; }
            memcpy(temp_buf, ptr0, num);
            temp_buf[num] = '\0';
            hour = temp_buf;

            ++ptr1;
            ptr0 = ptr1;
            int num = 0;
            while( *ptr1 != ':' ) { ++ptr1; ++num; }
            memcpy(temp_buf, ptr0, num);
            temp_buf[num] = '\0';
            hour = temp_buf;*/

        }
         
        // price realte -----
        p0 = ++p1;
        count = 0;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0'; 
            price = strbuf;
        }
        // change_price realte -----
        p0 = ++p1;
        count = 0;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0'; 
            change_price = strbuf;
        }
        // vol realte -----
        p0 = ++p1;
        count = 0;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0'; 
            vol = strbuf;
        }
        // amount realte -----
        p0 = ++p1;
        count = 0;
        while( *p1 != '\0' && int(*p1) != 0x0D && *p1 != ' ' ) { ++p1; ++count;}
        if( *p1 == '\0' ) break; 
        if( count > 0 )
        { 
            memcpy(strbuf, p0, count);
            strbuf[count] = '\0'; 
            amount = strbuf;
        }

        // filter 0x0A
        if( *p1 != '\0' )
            ++p1;
    } 
}

int main()
{
#ifdef TEST_OHTER
    testFenbiStrMatch();
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
#if 0
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
        }else if( args[0] == "FENBI" ) // FENBI  stock start_date end_date  --yyyymmdd
        {
            if( args.size() < 3 )
            {
                printf("command not corrent!\n");
                continue;
            }
            int date = 0;
            try
            {
                std::stoi(args[1]);
                date = std::stoi(args[2]);
            }catch(...)
            {
                continue;
            }
            static std::vector<T_FenbiCallBack> callbk_obj_vector;
            callbk_obj_vector.clear();
            T_FenbiCallBack  call_back_obj;
            callbk_obj_vector.push_back(call_back_obj);

            callbk_obj_vector[0].call_back_func = FenbiCallBackFun;
            auto val_ret = WinnerHisHq_GetHisFenbiData(const_cast<char*>(args[1].c_str()), date, &callbk_obj_vector[0], error);
            if( val_ret != 0 )
                std::cout << " WinnerHisHq_GetHisFenbiData fail: " << error << std::endl;
        }else 
        {
             printf("Can't recognize this command!\n");
        }

    }

#if 1
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
#endif
    //getchar();
    return 0;
}

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end, void *para)
{
    if( quote_data )
    {
        printf("%s %d:%d %.2f %.2f %d\n", quote_data->code, quote_data->date, quote_data->time, quote_data->price, quote_data->price_change, quote_data->vol);
        
    }
    if( is_end )
        is_end = is_end;
}