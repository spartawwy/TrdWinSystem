#include "quota_svr_common.h"
 
bool IsShStock(const std::string& code)
{
    if( code.length() == 6 && code.at(0) == '6' )
        return true;
    else 
        return false;
}

int TimeToLongday(const Time& t_m)
{
    auto tm_point = TSystem::MakeTimePoint((time_t)t_m.time_value(), t_m.frac_sec());
    TSystem::TimePoint t_p(tm_point);
    return TSystem::ToLongdate(t_p.year(), t_p.month(), t_p.day());
}

// 0--sunday  1--monday ...
int GetWeekDay(int date)
{
    struct tm  date_tm;
    date_tm.tm_year = date/10000;
    date_tm.tm_mon = (date%10000/100);
    date_tm.tm_mday = date%100;
    date_tm.tm_hour = 1;
    //time(&timep); /*获得time_t结构的时间，UTC时间*/

    time_t date_time = mktime(&date_tm);

    struct tm p;
    gmtime_s(&p, &date_time); /*转换为struct tm结构的UTC时间*/
    return p.tm_wday; //since sunday [0,6]
     
}

bool IsSameWeek(int date_a, int date_b)
{
    static auto get_yday = [](int date)->int
    {
        struct tm  date_tm;
        date_tm.tm_year = date/10000;
        date_tm.tm_mon = (date%10000/100);
        date_tm.tm_mday = date%100;
        date_tm.tm_hour = 1; 
        time_t date_time = mktime(&date_tm);
        struct tm p;
        gmtime_s(&p, &date_time); /*转换为struct tm结构的UTC时间*/
        return p.tm_yday; //since januray 1 [0,365]
    };

    if( date_a == date_b )
        return true;
    int b = std::max(date_a, date_b);
    int s = std::min(date_a, date_b);

    int diff = get_yday(b) - get_yday(s);
    if( diff > 6 )
       return false;
    int week_day_b = GetWeekDay(b);
    int week_day_s = GetWeekDay(s);

    // 0-- sunday 1--monday ...6--suturday
    switch(week_day_s)
    {
    case 0: 
        return false;
    case 1:
        return true;
    case 2: 
        return diff < 6;
    case 3:
        return diff < 5;
    case 4:
        return diff < 4;
    case 5: 
        return diff < 3;
    case 6:
        return diff < 2;
    }
    return false;
}