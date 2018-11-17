#ifndef QUOTA_SVR_COMMON_H_DSF3SDF_
#define QUOTA_SVR_COMMON_H_DSF3SDF_

#include <string>
#include <TLib/core/tsystem_time.h>

extern bool IsShStock(const std::string& code);
extern int TimeToLongday(const Time& t_m);
extern int GetWeekDay(int date);
extern bool IsSameWeek(int date_a, int date_b);
enum class KLINE_TYPE : unsigned char{KTYPE_HOUR, KTYPE_DAY, KTYPE_WEEK};
#endif // QUOTA_SVR_COMMON_H_DSF3SDF_