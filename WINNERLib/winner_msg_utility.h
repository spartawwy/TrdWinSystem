#ifndef WINNER_MSG_UTILITY_SDFDSFSD87_H_
#define WINNER_MSG_UTILITY_SDFDSFSD87_H_

#include <string>
//#include <ctime>
#include "winner_user_msg.pb.h"
#include "quotation_msg.pb.h"

namespace WINNERSystem
{
    void timeVal2YMDhms(time_t val, int *ymd, int *hms); 
    std::string QuotationReqType2Str(const QuotationReqType ret_type);
    std::string QuotationFqType2Str(const QuotationFqType fq_type);
    std::string QuotationRequest2Str(const QuotationRequest &quotation_request);

    
}
#endif