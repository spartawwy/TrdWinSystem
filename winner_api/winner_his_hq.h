#ifndef WINNER_HIS_HQ_SDF23DFSD_H_
#define WINNER_HIS_HQ_SDF23DFSD_H_

#define DllExport __declspec(dllexport)

extern "C" DllExport bool __stdcall WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo);
 
extern "C" DllExport void __stdcall WinnerHisHq_Disconnect();

#endif // WINNER_HIS_HQ_SDF23DFSD_H_