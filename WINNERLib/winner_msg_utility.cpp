#include "winner_msg_utility.h"

#include <ctime>
#include <tuple>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

void WINNERSystem::timeVal2YMDhms(time_t val, int *ymd, int *hms)
{
    struct tm p;
    localtime_s(&p, &val);  
    int yyyymmdd = (1900 + p.tm_year) * 10000 + (1 + p.tm_mon) * 100 +  p.tm_mday;
    int hhmmss = p.tm_hour * 10000 + p.tm_min * 100 + p.tm_sec;
    if( ymd )
        *ymd = yyyymmdd;
    if( hms )
        *hms = hhmmss;
} 

std::string WINNERSystem::QuotationReqType2Str(const QuotationReqType req_type)
{
    switch (req_type)
    {
    case  QuotationReqType::FENBI:            return "FENBI";
    case  QuotationReqType::ONE_MINUTE:       return "ONE_MINUTE";
    case  QuotationReqType::FIVE_MINUTE:      return "FIVE_MINUTE";
    case  QuotationReqType::THIRTY_MINUTE:    return "THIRTY_MINUTE";
    case  QuotationReqType::HOUR:             return "HOUR";
    case  QuotationReqType::DAY:              return "DAY";
    case  QuotationReqType::WEEK:             return "WEEK";
    case  QuotationReqType::MONTH:            return "MONTH";
    default: return "unknow QuotationReqType";
    }
}

std::string WINNERSystem::QuotationFqType2Str(const QuotationFqType fq_type)
{
    switch(fq_type)
    {
    case QuotationFqType::FQ_NO:      return "nofq";
    case QuotationFqType::FQ_BEFORE:  return "qfq";
    case QuotationFqType::FQ_AFTER:   return "hfq";
    default: return "";
    }
}

std::string WINNERSystem::QuotationRequest2Str(const QuotationRequest &req)
{
    int yyyymmdd_beg = 0;
    int hhmmss_beg = 0;
    int yyyymmdd_end = 0;
    int hhmmss_end = 0;
    WINNERSystem::timeVal2YMDhms(req.beg_time().time_value(), &yyyymmdd_beg, &hhmmss_beg);
    WINNERSystem::timeVal2YMDhms(req.end_time().time_value(), &yyyymmdd_end, &hhmmss_end);
  
    return TSystem::utility::FormatStr("%u %s %s %d-%d %d-%d %d %s"
        , req.req_id()
        , QuotationReqType2Str(req.req_type()).c_str()
        , req.code().c_str()
        , yyyymmdd_beg
        , hhmmss_beg
        , yyyymmdd_end
        , hhmmss_end
        , req.is_index()
        , QuotationFqType2Str(req.fq_type()).c_str());
 
}