#ifndef WINNER_HIS_HQ_SDF23DFSD_H_
#define WINNER_HIS_HQ_SDF23DFSD_H_

#ifdef  WINNER_API_EXPORTS  
#define DLLIMEXPORT __declspec(dllexport) 
#else
#define DLLIMEXPORT __declspec(dllimport)
#endif

extern "C" DLLIMEXPORT int __cdecl WinnerHisHq_Connect(char* IP, int Port, char* Result, char* ErrInfo);

extern "C" DLLIMEXPORT void __cdecl WinnerHisHq_Disconnect();

#endif // WINNER_HIS_HQ_SDF23DFSD_H_