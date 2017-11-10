#ifndef WINNER_ERROR_H_SDFDS3DSF_
#define WINNER_ERROR_H_SDFDS3DSF_

#include "TLib/core/tsystem_error.h"

namespace WINNERSystem
{
    class WINNERErrorCategory : public TSystem::TErrorCategory
    {
    public:

        enum class ErrorCode : short
        {
            OK = 0,

            INVALID_LOGIN,

            BAD_STOCK_CODE,
        };

        static WINNERErrorCategory& GetCategory();

        virtual const std::string& name() const override;

		virtual bool CheckOK(short ec) const override;

		virtual const std::string& Message(short ec) const override;

	private:

		WINNERErrorCategory();

    };

    // make a TError based on WZEErrorCategory
	TSystem::TError     MakeTError( WINNERErrorCategory::ErrorCode ec );

	// make a TException based on WZEErrorCategory
	TSystem::TException MakeTException( WINNERErrorCategory::ErrorCode ec );
}
#endif //WINNER_ERROR_H_SDFDS3DSF_