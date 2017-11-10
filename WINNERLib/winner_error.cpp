#include "winner_error.h"

using namespace WINNERSystem;

WINNERErrorCategory& WINNERErrorCategory::GetCategory()
{
    static WINNERErrorCategory error_cate_;
	return error_cate_;
}

WINNERErrorCategory::WINNERErrorCategory()
{ 
}
 
const std::string& WINNERErrorCategory::name() const
{
	static const std::string name_("WINNERErrorCategory");
	return name_;
}

bool WINNERErrorCategory::CheckOK(short ec) const
{
	return ec == static_cast<short>(ErrorCode::OK);
}


const std::string& WINNERErrorCategory::Message(short ec) const
{
	static const std::string bad_ec("not this error category");
	static const std::string ok("ok");
	static const std::string invalid_login("invalid login");
	static const std::string bad_stock_code("bad stock code");

    switch (ec)
	{
	case ErrorCode::OK:
		return ok;
	case ErrorCode::INVALID_LOGIN:
		return invalid_login;
    case ErrorCode::BAD_STOCK_CODE:
        return bad_stock_code;

    default:
		return bad_ec;
    }

}

TSystem::TError WINNERSystem::MakeTError( WINNERErrorCategory::ErrorCode ec )
{
	return TSystem::TError(static_cast<short>(ec), WINNERErrorCategory::GetCategory());
}

TSystem::TException WINNERSystem::MakeTException( WINNERErrorCategory::ErrorCode ec )
{
	return TSystem::TException( MakeTError(ec) );
}