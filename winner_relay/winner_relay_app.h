#ifndef EXCHANGE_RELAY_H_
#define EXCHANGE_RELAY_H_

#include <TLib/tool/tsystem_server_appbase.h>

#include "forward_module.h"
#include "heartbeat_module.h"
#include "master_selection_module.h"
#include "log_msg_module.h"
//#include "info_msg_module.h"
//#include "raid_state_module.h"

class WinnerRelayApp
	: public TSystem::ServerAppBase
{
public:

	WinnerRelayApp(const std::string& name, const std::string& version);
	virtual ~WinnerRelayApp();

	void Initiate();
	virtual void Shutdown() override;

private:

	virtual void UpdateState();

	virtual void HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override {}
	virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override {}
	virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn
		, const TSystem::TError& te) override;

	HeartbeatModule		heartbeat_module_;

	MasterSelectionModule	master_selection_module_;

	ForwardModule		forward_module_;

	LogMsgModule		log_msg_module_;

    /*InfoMsgModule       info_msg_module_;

    RaidStateModule     raid_state_module_;*/

	friend class HeartbeatModule;
	friend class ForwardModule;
	friend class LogMsgModule;
    friend class InfoMsgModule;
    friend class RaidStateModule;
};

#endif // EXCHANGE_RELAY_H_