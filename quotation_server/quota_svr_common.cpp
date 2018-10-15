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