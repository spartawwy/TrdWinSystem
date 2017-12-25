#ifndef WZF_CLIENT_SOCKET_H_
#define WZF_CLIENT_SOCKET_H_

#include <memory>
#include <string>

#include <TLib/core/tsystem_serialization.h>
#include <TLib/tool/tsystem_rational_number.h>
#include <TLib/tool/tsystem_server_client_appbase.h>

//#include "WZELib/wze_private_qr_msg.pb.h"
//#include "WZELib/wze_query_msg.pb.h"
//#include "WZELib/wze_transfer_msg.pb.h"

namespace TSystem
{
	namespace communication
	{
		class Connection;
	}
}
//
//struct TPrivateQuoteResponseField
//{
//	unsigned int     user_id;
//	SymbolVariable   symbol_var;
//	std::string      qr_id;
//	std::string      quote_id;
//	unsigned int     quantity;
//	RationalNumber   price;
//	std::string      buy_sell;
//	std::string      open_close;
//	std::string      remark;
//	TPrivateQuoteResponseField(): user_id(-1), symbol_var(), qr_id(), quote_id(), quantity(0)
//		, price(), buy_sell(), open_close(), remark()
//	{
//	}
//};

class WZFClientSocket
{
public:

	// constructor
	WZFClientSocket(TSystem::ServerClientAppBase& app);

	// connection
	void Connection(const std::shared_ptr<TSystem::communication::Connection>& pconn);

	// send message
	void AsyncSendMessage(const google::protobuf::MessageLite& message)
	{ 
		assert( pconn_ );
		pconn_->AsyncSend( Encode(message, app_.msg_system(), TSystem::Message::HeaderType(0, app_.pid(), 0)) );
	}

	// send user request <LOGIN or LOGOUT>
	void SendUserRequest(unsigned int request_id, RequestType type, unsigned int user_id, const std::string& password);

	// send quote request
	void SendPrivateQuoteRequest(unsigned int user_id, const SymbolVariable& symbol_var
		, unsigned int quantity, const std::string& buy_sell, const std::string& open_close, const std::string* remark=nullptr);
	void CancelPrivateQuoteRequest(unsigned int user_id, const SymbolVariable& symbol_var, const std::string& qr_id);

	// send quote response
	void SendPrivateQuoteResponse(TPrivateQuoteResponseField& field);
	void CancelPrivateQuoteResponse(unsigned int user_id, const SymbolVariable& symbol_var, const std::string& quote_id);

	// send quote book
	void SubmitPrivateQuoteBook(TPrivateQuoteResponseField& field);

	// send query request
	void SendQueryRequest(unsigned int user_id, QueryType query_type, const SymbolVariable* symbol_var=nullptr
		, const QueryRequest::QueryStatusType* p_status=nullptr);

	// send query response
	void SendQueryServerResponse(unsigned int user_id, const std::string& qr_id);

	// send transfer
	void SubmitTransfer(unsigned int user_id, const SymbolVariable& symbol_var, const std::string& user_trsf_ref, const std::string& trsf_type
		, const RationalNumber& quantity, const std::string& account_category, const std::string* remark=nullptr);
	void ConfirmTransfer(unsigned int user_id, const SymbolVariable& symbol_var, const std::string& exch_trsf_ref, const std::string& trsf_type
		, const RationalNumber& quantity, const std::string& account_category);

	// send quote trial
	void SendPrivateQuoteTrial(unsigned int user_id, const SymbolVariable& symbol_var, unsigned int quantity, const RationalNumber& price, const std::string& buy_sell, const std::string& open_close);

private:

	TSystem::ServerClientAppBase&							app_;
	std::shared_ptr<TSystem::communication::Connection>		pconn_;
};

#endif // WZF_CLIENT_SOCKET_H_