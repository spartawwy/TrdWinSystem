#include "quota_svr_common.h"

bool IsShStock(const std::string& code)
{
    if( code.length() == 6 && code.at(0) == '6' )
        return true;
    else 
        return false;
}
