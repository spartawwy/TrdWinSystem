#ifndef WINNER_HIS_HQ_SDF23DFSD_H_
#define WINNER_HIS_HQ_SDF23DFSD_H_

#include "winner_hq_api.h"

#ifdef  WINNER_API_EXPORTS  
#define DLLIMEXPORT __declspec(dllexport) 
#else
#define DLLIMEXPORT __declspec(dllimport)
#endif

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo);

extern "C" DLLIMEXPORT void __cdecl WinnerHisHq_Disconnect();

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiData(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo);
extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetHisFenbiDataBatch(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char* ErrInfo);

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_GetKData(char* Zqdm, PeriodType type, int date_begin, int date_end
                                                           , T_KDataCallBack *call_back_para, bool is_index, char* ErrInfo);
#endif // WINNER_HIS_HQ_SDF23DFSD_H_