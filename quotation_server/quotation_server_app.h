#ifndef QUOTATION_SERVER_APP_H_SDF55SDSF_
#define QUOTATION_SERVER_APP_H_SDF55SDSF_

#include <TLib/tool/tsystem_server_appbase.h>
 
//#include "db_moudle.h"

using namespace TSystem;

class QuotationServerApp : public ServerAppBase
{
public:

	QuotationServerApp(const std::string &name, const std::string &version);
	virtual ~QuotationServerApp();

	virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override 
	{
	};
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
		, const TError& te) override {};

	virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;
	virtual void UpdateState() override;

	void Initiate();

private:

	void InitPython();
};
#endif // QUOTATION_SERVER_APP_H_SDF55SDSF_