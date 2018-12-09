#include <TLib/core/tsystem_time.h>

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

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiDataBatch(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char *ErrInfo)
{
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestFenbiHisDataBatch(Zqdm, date_begin, date_end, call_back_para, ErrInfo);
    return val ? 0 : -2;
}

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetKData(char* Zqdm, PeriodType type, int date_begin, int date_end
                                                           , T_KDataCallBack *call_back_para, bool is_index, char *ErrInfo)
{
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestKData(Zqdm, type, date_begin, date_end, call_back_para, is_index, ErrInfo);
    return val ? 0 : -2;
}

extern "C" DLLIMEXPORT int  WinnerHisHq_GetQuote(char* Zqdm, int Date, int hhmmss, T_QuoteAtomData *ret_quote_data, char* ErrInfo)
{ 
    auto from_quote_data = [&](WinnerClient::TTimeMapQuoteData * p_time_his_quote)->bool
    {
        assert(p_time_his_quote);
        auto time_point = TSystem::MakeTimePoint(Date/10000, Date%10000/100, Date%100, hhmmss/10000, hhmmss%10000/100, hhmmss%100, 0);
        std::time_t time_val = std::chrono::system_clock::to_time_t(time_point);
        auto iter = p_time_his_quote->find(time_val);
        if( iter == p_time_his_quote->end() )
            iter = p_time_his_quote->find(time_val-1);
        if( iter == p_time_his_quote->end() )
            iter = p_time_his_quote->find(time_val-2);
        if( iter != p_time_his_quote->end() )
        {
            WinnerClient::QuoteAtomData *quote_data = iter->second.get();
            if( ret_quote_data )
                memcpy(ret_quote_data, std::addressof(quote_data->data_), sizeof(T_QuoteAtomData));
            return true;
        }else
            return false;
    };

    WinnerClient::TTimeMapQuoteData * p_time_his_quote = GetInstance()->FindHisQuote(Zqdm, Date);

    if( p_time_his_quote )
    {
        if( from_quote_data(p_time_his_quote) )
            return 0;
        else
            return -2;
    }
    if( !GetInstance()->is_connected() )
        return -1;
    auto val = GetInstance()->RequestHisQuote(Zqdm, Date, ErrInfo);

    p_time_his_quote = GetInstance()->FindHisQuote(Zqdm, Date);
    if( p_time_his_quote )
    {
        if( from_quote_data(p_time_his_quote) )
            return 0;
        else
            return -2;
    }else
    {
        if( ErrInfo )
            strcpy(ErrInfo, "no related data");
        return -2;
    }
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

