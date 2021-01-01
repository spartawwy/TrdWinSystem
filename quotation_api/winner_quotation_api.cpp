#include "winner_quotation_api.h"

#include <thread>
#include <windows.h>

#include "winner_quotation.h"

static WinnerQuotation* GetInstance(bool is_del=false);

extern "C" DLLIMEXPORT int __cdecl WinnerQuotation_Reg(char* Zqdm, int date_begin, int date_end
                                                        , QuoteCallBackData *call_back_data, bool is_index, char *ErrInfo)
{
    /*if( !GetInstance()->is_connected() )
        return -1;*/
    //auto val = GetInstance()->RequestKData(Zqdm, date_begin, date_end, call_back_para, is_index, ErrInfo);
    if( !call_back_data )
        return -1;

    auto val = GetInstance()->Register(call_back_data->quote_call_back, call_back_data->each_delay_ms, Zqdm
                                       , date_begin, 901);
    return val ? 0 : -2;
}

extern "C" DLLIMEXPORT int __cdecl WinnerQuotation_Start()
{
    /*auto val = GetInstance()->RequestKData(Zqdm, date_begin, date_end, call_back_para, is_index, ErrInfo);
    return val ? 0 : -2;*/
    return 1;
}

extern "C" DLLIMEXPORT void __cdecl WinnerQuotation_Stop()
{

}


static WinnerQuotation* GetInstance(bool is_del)
{
    //static std::string pro_tag = TSystem::utility::ProjectTag("wzf");
    static WinnerQuotation * winn_quotation_obj = nullptr; 

    if( is_del )
    {
        if( winn_quotation_obj ) 
        {
            //winn_client_obj->FireShutdown(); 
            winn_quotation_obj->Shutdown(); 
        }
    }else if( !winn_quotation_obj )
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
            winn_quotation_obj = new WinnerQuotation;
            winn_quotation_obj->MainLoop();

            delete winn_quotation_obj; 
            winn_quotation_obj = nullptr;
        });
#endif
        man_thread.detach();
    }

    // wait for winn_quotation_obj created 
    while( !is_del && !winn_quotation_obj )
        Sleep(10);
    return winn_quotation_obj;
}

