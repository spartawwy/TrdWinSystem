#ifndef FORWARD_MODULE_H_
#define FORWARD_MODULE_H_

#include <mutex>
#include <string>
#include <unordered_map>

#include <TLib/core/tsystem_connection.h>
#include <TLib/core/tsystem_serialization.h>

class WinnerRelayApp;

class ForwardModule
{
public:

	explicit ForwardModule(WinnerRelayApp& app);

	void Initiate();

	// add connection
	void AddConnection(unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn);

	// check and forward msg, return if forwarded
	bool CheckForward(const TSystem::Message& msg);

private:

	WinnerRelayApp&			app_;

	// pid of system relay of other nodes
	std::unordered_map<std::string, unsigned short>		nodes_;

	// <pid -> connection> of this node
	std::unordered_map<unsigned short
		, std::shared_ptr<TSystem::communication::Connection>>		connections_;
	std::mutex                                                      connections_mutex_;
};

#endif // FORWARD_MODULE_H_