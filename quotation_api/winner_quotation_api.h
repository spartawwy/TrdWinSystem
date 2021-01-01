#ifdef  QUOTATION_API_EXPORTS  
#define DLLIMEXPORT __declspec(dllexport) 
#else
#define DLLIMEXPORT __declspec(dllimport)
#endif

typedef struct _t_quote_data
{
    // 'code','date','time','price','change','volume','amount','type'
    char code[16];
    //int  date; //yyyymmdd
    __int64  time; //HHMMSS
    double price; // .2f
    //double price_change;
    //int  vol;
    //unsigned char bid_type; // 0: buy_pan  1 :sell_pan
    double b_1;
    int v_b_1;
    double b_2;
    double b_3;
    double b_4;
    double b_5;
    double s_1;
    int v_s_1;
    double s_2;
    double s_3;
    double s_4;
    double s_5; 
}T_QuoteData;

typedef void (*QuoteCallBack)(T_QuoteData *quote_data, void *para);

typedef struct quote_call_back_data_
{
    QuoteCallBack quote_call_back;
    unsigned int  each_delay_ms;
}QuoteCallBackData;



extern "C" DLLIMEXPORT int __cdecl WinnerQuotation_Reg(char* Zqdm, int date_begin, int date_end
                                                        , QuoteCallBackData *call_back_data, bool is_index, char *ErrInfo);

extern "C" DLLIMEXPORT int __cdecl WinnerQuotation_Start();

extern "C" DLLIMEXPORT void __cdecl WinnerQuotation_Stop();