#ifdef TEST_QUOTAION_API

#include <cstdio>
#include <windows.h>

#include "..\quotation_api\winner_quotation_api.h"

#pragma comment(lib, "quotation_api.lib")

//typedef void (*QuoteCallBack)(T_QuoteData *quote_data, void *para);
void QuoteCallBackFunc(T_QuoteData *quote_data, void *para)
{
    if( quote_data )
        printf("code:%s price:%f \n", quote_data->code, quote_data->price);
}

int main()
{
    char error[256] = {'\0'};
    QuoteCallBackData call_back_data;
    call_back_data.quote_call_back = QuoteCallBackFunc;
    call_back_data.each_delay_ms = 50;
    WinnerQuotation_Reg("sc1903", 20190101, 20190229
        , &call_back_data, false, error);

    while( true )
    {
        Sleep(500);
    }
    return 0;
}
#endif