#include "sys_common.h" 
 
#include <TLib/core/tsystem_time.h>
#include <chrono>
#include <thread>

#include <ctime>
 
#include <TLib/core/tsystem_utility_functions.h> 


void Delay(unsigned short mseconds)
{
	//TSystem::WaitFor([]()->bool { return false;}, mseconds); // only make effect to timer
	std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));

}
 
 
TypeMarket GetStockMarketType(const std::string &stock_code)
{
	return (stock_code.c_str()[0] == '6' ? TypeMarket::SH : TypeMarket::SZ);
}

//bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
//{
//	return lh.id < rh.id; 
//}

bool Equal(double lh, double rh)
{
	return fabs(lh-rh) < 0.0001;
}

//QTime Int2Qtime(int val)
//{
//	return QTime(val / 10000, (val % 10000) / 100, val % 100);
//}

bool IsStrAlpha(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalpha(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}

}

bool IsStrNum(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalnum(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}

}

std::string TagOfLog()
{
	return TSystem::utility::FormatStr("app_%d", TSystem::Today());
	//return  "OrderData_" + TSystem::Today()
}

std::string TagOfOrderLog()
{
	return TSystem::utility::FormatStr("OrderData_%d", TSystem::Today());
	//return  "OrderData_" + TSystem::Today()
}

std::tuple<int, std::string> CurrentDateTime()
{
	time_t rawtime;
	time(&rawtime);

	const int cst_buf_len = 256;
	char szContent[cst_buf_len] = {0};

	struct tm * timeinfo = localtime(&rawtime);
	sprintf_s( szContent, cst_buf_len, "%02d:%02d:%02d"
		, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec ); 

	return std::make_tuple((timeinfo->tm_year + 1900) * 10000 + (timeinfo->tm_mon + 1) * 100 + timeinfo->tm_mday
		, std::string(szContent));

}

bool IsNowTradeTime()
{
	return true; //temp code
	static auto get_date = []()
	{
		time_t rawtime;
		struct tm * timeinfo;
		time( &rawtime );
		timeinfo = localtime( &rawtime ); // from 1900 year
		return timeinfo->tm_year * 10000 + timeinfo->tm_mon *100 + timeinfo->tm_mday;
	};


	static int week_day = 0;  
	static int ori_day = 0;
	static time_t sec_beg = 0;
	static time_t sec_rest_beg = 0;
	static time_t sec_rest_end = 0;
	static time_t sec_end = 0;

	time_t rawtime = 0;
	struct tm * timeinfo = nullptr;
	time( &rawtime );

	auto cur_day = get_date();
	if( ori_day != cur_day )
	{
		ori_day = cur_day;

		timeinfo = localtime( &rawtime ); // from 1900 year
		week_day = timeinfo->tm_wday;

		struct tm tm_trade_beg;
		tm_trade_beg.tm_year = timeinfo->tm_year;
		tm_trade_beg.tm_mon = timeinfo->tm_mon;
		tm_trade_beg.tm_mday = timeinfo->tm_mday;
		tm_trade_beg.tm_hour = 9;
		tm_trade_beg.tm_min = 25;
		tm_trade_beg.tm_sec = 59;
		sec_beg = mktime(&tm_trade_beg);

		struct tm tm_rest_beg; 
		tm_rest_beg.tm_year = timeinfo->tm_year;
		tm_rest_beg.tm_mon = timeinfo->tm_mon;
		tm_rest_beg.tm_mday = timeinfo->tm_mday;
		tm_rest_beg.tm_hour = 11;
		tm_rest_beg.tm_min = 32;
		tm_rest_beg.tm_sec = 00;
		sec_rest_beg = mktime(&tm_rest_beg);

		struct tm tm_rest_end; 
		tm_rest_end.tm_year = timeinfo->tm_year;
		tm_rest_end.tm_mon = timeinfo->tm_mon;
		tm_rest_end.tm_mday = timeinfo->tm_mday;
		tm_rest_end.tm_hour = 12;
		tm_rest_end.tm_min = 58;
		tm_rest_end.tm_sec = 00;
		sec_rest_end = mktime(&tm_rest_end);

		struct tm tm_trade_end; 
		tm_trade_end.tm_year = timeinfo->tm_year;
		tm_trade_end.tm_mon = timeinfo->tm_mon;
		tm_trade_end.tm_mday = timeinfo->tm_mday;
		tm_trade_end.tm_hour = 15;
		tm_trade_end.tm_min = 32;
		tm_trade_end.tm_sec = 59;
		sec_end = mktime(&tm_trade_end);
	}

	if( week_day == 6 || week_day == 0 ) // sunday: 0, monday : 1 ...
		return false; 
	if( (rawtime >= sec_beg && rawtime <= sec_rest_beg) || (rawtime >= sec_rest_end && rawtime <= sec_end) )
		return true;
	else 
		return false;
}

std::chrono::system_clock::time_point Int2TimePoint(int val)
{
	// ndedt:  todo:
	 //return  TSystem::GetClock().Now();
	return std::chrono::system_clock::time_point();
}

 //bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
 //{
	// return lh.id < rh.id; 
 //}

 