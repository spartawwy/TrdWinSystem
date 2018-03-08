#ifndef INTTERMFACE_SDF3DKDFS_H_
#define INTTERMFACE_SDF3DKDFS_H_

#include <memory>
#include <vector>

#include <TLib/tool/tsystem_async_operation.h>
#include <TLib/tool/tsystem_server_client_appbase.h>

#include "wzf_client_socket.h"

class PrivateQuoteMessage;
class PrivateQuoteMessageAck;

class Intermface: public TSystem::ServerClientAppBase
{
public:

	Intermface(bool monitor_flag);

	virtual ~Intermface();

	void Initiate();

	void Shutdown();

	void Start(const std::string& res_name);

	void ParseCmd();

	void ProcessCmd(const std::vector<std::string>& cmd);

private:

	virtual void HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;

	void Login(unsigned int user_id, const std::string& password);

    void RequestQuotation(const std::string &code);

private:
	// the default handlers
	TSystem::MessageHandlerGroup                           msg_handlers_;

	// the connection
	std::shared_ptr<TSystem::communication::Connection>    pconn_;

	// the async operation
	TSystem::AsyncOperationManager                         async_ops_;

	WZFClientSocket							               wzf_client_socket_;
	bool												   monitor_flag_;
}; 
#endif // INTTERMFACE_SDF3DKDFS_H_