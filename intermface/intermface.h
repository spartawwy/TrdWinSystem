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

}; 
#endif // INTTERMFACE_SDF3DKDFS_H_