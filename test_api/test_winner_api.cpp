
#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>
#include <vector>

#include <regex>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>
#include <Boost/lexical_cast.hpp>

#include <Windows.h>

#include "winner_hq_api.h"

#define  TEST_API
//#define  TEST_OHTER

#ifdef TEST_OHTER
using namespace TSystem;
#endif

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end, void *para);
void KDataCallBackFun(T_K_Data *k_data, bool is_end, void *para);

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
    strcpy_s(buf, sizeof(buf), "13908 14:59:41 38.65 -0.01 14 54110\n13909 14:59:45 38.65 0.0 10 38650\n");

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

void Test_GetFenbi(const std::string &start, const std::string &end, const std::string &code);
void Test_GetHisData(const std::string &start, const std::string &end, const std::string &code, PeriodType period_type);

static int bar_daystr_to_longday(const std::string &day_str);
static bool is_sh_stock(const std::string& code);

int main()
{  
   // Test_GetFenbi("2018-07-02", "2018-07-03", "600196");
    
    //-------------------
#if 0  //simple test
    char content[] = "2018-8-1,09:25:03,37.29,606,606,2259774,37.14,14,37.11,20,37.06,2,37.03,32,37.02,27,37.29,139,37.30,16,37.31,14,37.32,13,37.33,89,B";
    char *p0 = &content[0];
    char *p1 = &content[sizeof(content)-1];
    std::string src(p0, p1);
    
    //这里 "()" 用于捕获组, 捕获组的编号是按照 "(" 出现的顺序, 从左到右, 从1开始进行编号的  
    //std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}:\\d{2}:\\d{2}),([0-9]+\\.[0-9]+),([0-9]+)(.*)$";
    std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}:\\d{2}:\\d{2}),(\\d+\\.\\d+),(\\d+)(.*)$"; 
    std::regex regex_obj(partten_string); 

    std::smatch result; 
    if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
    {
        std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
    }
#endif 
#ifdef TEST_MATCH
    testFenbiStrMatch();
      
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

#ifdef TEST_API
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
    WinnerHisHq_GetKDataDelegate  WinnerHisHq_GetKData
        = (WinnerHisHq_GetKDataDelegate)GetProcAddress(api_handle, "WinnerHisHq_GetKData"); 
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
        
        TSystem::utility::replace_all(cmd, "  ", " ");
        TSystem::utility::replace_all(cmd, "  ", " ");
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
        }else if( args[0] == "KDATA" ) // KDATA  stock period_type(2:5m, 3:15m, 4:30m, 5:1hour ,6:day,7:week,8:mon) start_date end_date is_index --yyyymmdd
        {
            if( args.size() < 6 )
            {
                printf("command not corrent!\n");
                continue;
            } 
            PeriodType  period_type; 
            int start_date = 0;
            int end_date = 0;
            bool is_index = false;
            try
            { 
                period_type = (PeriodType)std::stoi(args[2]);
                start_date = std::stoi(args[3]);
                end_date = std::stoi(args[4]);
                is_index = (bool)std::stoi(args[5]);
            }catch(...)
            {
                continue;
            }
            static std::vector<T_KDataCallBack> kdata_callbk_obj_vector;
            kdata_callbk_obj_vector.clear();
            T_KDataCallBack  call_back_obj;
            kdata_callbk_obj_vector.push_back(call_back_obj);

            kdata_callbk_obj_vector[0].call_back_func = KDataCallBackFun;
   auto val_ret = WinnerHisHq_GetKData(const_cast<char*>(args[1].c_str()), period_type, start_date, end_date, &kdata_callbk_obj_vector[0], is_index, error);
            if( val_ret != 0 )
                std::cout << " WinnerHisHq_GetKData fail: " << error << std::endl;
             
        }else 
        {
             printf("Can't recognize this command!\n");
        }

    }

    //Test_GetHisData("600196", "2018-07-02", "2018-07-03", PeriodType::PERIOD_HOUR);

#if 1
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
#endif
#endif // TEST_API

    getchar();
    return 0;
}

void Test_GetFenbi(const std::string &start, const std::string &end, const std::string &code)
{
   
#if 0
    char buffer[256] = {0};
    std::string full_path = "D:/ProgramData/Stock_Data/201808/20180801/20180801SH/600196_20180801.csv";
    std::ifstream in(full_path, std::ios::in);
    if( !in.is_open() )
    {
        printf("open file\n");
        getchar();
        return 0;
    }
    while( !in.eof() )
    {
        in.getline(buffer, sizeof(buffer));
        printf(buffer);
        printf("\n");
    }
#endif 
     //std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}:\\d{2}:\\d{2}),(\\d+\\.\\d+),(\\d+)(.*)$"; 
     std::string partten_string = "^(\\d{4}-\\d{1,2}-\\d{1,2}),(\\d{2}:\\d{2}:\\d{2}),(\\d+\\.\\d+),(\\d+)(.*)"; 
     std::regex regex_obj(partten_string); 

	 std::string stk_data_dir = "D:/ProgramFilesBase/StockData/";
     //std::string full_path = "D:/ProgramData/Stock_Data/201808/20180801/20180801SH/600196_20180801.csv";
#if 1
     //std::string src0 = "2018-7-2,09:25:01,41.38,408,408,1688304,41.36,20,41.30,102,41.25,5,41.23,105,41.20,115,41.38,5,41.39,15,41.40,212,41.41,20,41.42,69,B\r"; // bad
     std::string src0 = "2018-7-2,09:25:01,41.38,408,408,1688304,41.36,20,41.30,102,41.25,5,41.23,105,41.20,115,41.38,5,41.39,15,41.40,212,41.41,20,41.42,69,B";
	 std::smatch result0; 
     if( std::regex_match(src0.cbegin(), src0.cend(), result0, regex_obj) )
     {
        std::cout << result0[1] << " " << result0[2] << " " << result0[3] << " " << result0[4] << std::endl;
     }

#endif

	 int end_date = bar_daystr_to_longday(end);
     
	 auto end_date_com = TSystem::FromLongdate(end_date);

     static auto create_file_path = [](const std::string& stk_data_dir, const std::string& date_str, const std::string& code)
     {
         int date = bar_daystr_to_longday(date_str);
	     auto date_com = TSystem::FromLongdate(date);
         std::string date_format_str = TSystem::utility::FormatStr("%d%02d%02d", std::get<0>(date_com), std::get<1>(date_com), std::get<2>(date_com));
	     std::string year_mon = TSystem::utility::FormatStr("%d%02d", std::get<0>(date_com), std::get<1>(date_com));
         std::string year_mon_sub = year_mon + (is_sh_stock(code) ? "SH" : "SZ");
         return stk_data_dir + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + code + "_" + date_format_str + ".csv";
     };

#if 0
     int start_date = bar_daystr_to_longday(start);
	 auto start_date_com = TSystem::FromLongdate(start_date);
     std::string start_date_str = utility::FormatStr("%d%02d%02d", std::get<0>(start_date_com), std::get<1>(start_date_com), std::get<2>(start_date_com));
	 std::string start_year_mon = utility::FormatStr("%d%02d", std::get<0>(start_date_com), std::get<1>(start_date_com));
     std::string start_year_mon_sub = start_year_mon + (is_sh_stock(code) ? "SH" : "SZ");
#endif
	 std::string full_path = create_file_path(stk_data_dir, start, code);

     std::fstream in(full_path);
     if( !in.is_open() )
     {
         printf("open file fail");
         getchar();
         return;
     }
     char buf[256] = {0};
     while( !in.eof() )
     {
         in.getline(buf, sizeof(buf));
         int len = strlen(buf);
         if( len < 1 )
             continue;

         char *p0 = buf;
         char *p1 = &buf[len-1];
         std::string src(p0, p1);

         std::smatch result; 
         if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
         {
             std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
         }
     }

}

void Test_GetHisData(const std::string &start, const std::string &end, const std::string &code, PeriodType period_type)
{

}

void FenbiCallBackFun(T_QuoteAtomData *quote_data, bool is_end, void *para)
{
    if( quote_data )
    {
        //printf("%s %d:%d %.2f %.2f %d\n", quote_data->code, quote_data->date, quote_data->time, quote_data->price, quote_data->price_change, quote_data->vol);
    }
    if( is_end )
        is_end = is_end;
}

void KDataCallBackFun(T_K_Data *k_data, bool is_end, void *para)
{
    if( k_data )
    {
        //printf("%s %d:%d %.2f %.2f %d\n", quote_data->code, quote_data->date, quote_data->time, quote_data->price, quote_data->price_change, quote_data->vol);
    }
    if( is_end )
        is_end = is_end;
}

int bar_daystr_to_longday(const std::string &day_str)
{ 
    int year, mon, day;
    std::string partten_string = "^(\\d{4})-(\\d{1,2})-(\\d{1,2})$"; 
    std::regex regex_obj(partten_string); 
    std::smatch result; 
    if( std::regex_match(day_str.cbegin(), day_str.cend(), result, regex_obj) )
    {
        try
        {
            year = boost::lexical_cast<int>(result[1]);
            mon = boost::lexical_cast<int>(result[2]);
            day = boost::lexical_cast<int>(result[3]);
        }catch(boost::exception&)
        {
            return 0;
        }
        std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
    }

    return TSystem::ToLongdate(year, mon, day);
}

bool is_sh_stock(const std::string& code)
{
    if( code.length() == 6 && code.at(0) == '6' )
        return true;
    else 
        return false;
}