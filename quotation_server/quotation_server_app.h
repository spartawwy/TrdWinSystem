#ifndef QUOTATION_SERVER_APP_H_SDF55SDSF_
#define QUOTATION_SERVER_APP_H_SDF55SDSF_

#include <vector>

#include <TLib/tool/tsystem_server_appbase.h>

#include "WINNERLib/boost_locker.h"
#include "WINNERLib/winner_user_msg.pb.h"
#include "WINNERLib/quotation_msg.pb.h"
//#include "db_moudle.h"

using namespace TSystem;

class UserRequest; 
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
	void SetupServerSocket();
	void HandleUserRequest(std::shared_ptr<UserRequest>& req, std::shared_ptr<communication::Connection>& pconn);
	void HandleUserLogin(const UserRequest& req, const std::shared_ptr<communication::Connection>& pconn);

    void HandleQuotationRequest(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn);

	void SendRequestAck(int user_id, int req_id, RequestType type, const std::shared_ptr<TSystem::communication::Connection>& pconn);

    // date_beg : yyyyMMdd 
    std::vector<std::string> GetFenbi2File(const std::string &code, int date_beg, int date_end);

private:

    void *PyFuncGetAllFill2File;
    std::string stk_data_dir_;
    // ------------
    typedef std::unordered_map<int, std::shared_ptr<TaskStrand> > TConnidMapStrand;
    TConnidMapStrand conn_strands_;
    WRMutex  conn_strands_wr_mutex_; 
    // ------------

};
#endif // QUOTATION_SERVER_APP_H_SDF55SDSF_