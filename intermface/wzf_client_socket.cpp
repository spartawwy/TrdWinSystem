#include "wzf_client_socket.h"

#include <TLib/core/tsystem_time.h>

#include "WinnerLib/quotation_msg.pb.h"

//#include "WZELib/wze_private_qr_msg.h"
//#include "WZELib/wze_query_msg.h"
//#include "WZELib/wze_transfer_msg.h"
//#include "WZELib/wze_product.h"

using namespace TSystem;
//using namespace WZFSystem;

WZFClientSocket::WZFClientSocket(ServerClientAppBase& app)
	: app_(app)
	, pconn_(nullptr)
{
}

void WZFClientSocket::Connection(const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	assert( pconn );
	pconn_ = pconn;
}

void WZFClientSocket::SendUserRequest(unsigned int request_id, RequestType type, unsigned int user_id, const std::string& password)
{
	UserRequest req;
	req.set_request_id(request_id);
	req.set_request_type(type);
	req.set_user_id(user_id);
	req.set_password(password);

	AsyncSendMessage(req);
}

void WZFClientSocket::SendQuotationRequst(const std::string& code)
{
    QuotationRequest quotation_req;
    quotation_req.set_code(code);
     
    Time tm_beg;
    Time tm_end;
    FillTime(TimePoint(MakeTimePoint(2018, 3, 8)), *quotation_req.mutable_beg_time());
    FillTime(TimePoint(MakeTimePoint(2018, 3, 8)), *quotation_req.mutable_end_time());
    quotation_req.set_req_type(QuotationReqType::FENBI);
    AsyncSendMessage(quotation_req);
}
