#ifndef MASTER_SELECTION_MODULE_H_
#define MASTER_SELECTION_MODULE_H_

#include <map>
#include <vector>

#include <TLib/core/tsystem_connection.h>
#include <TLib/core/tsystem_topic_service.h>

class WinnerRelayApp;

//--------------------------
// select the master resource in a raid
// when connected, will send the RaidMaster message with master resource's pid
// when the master resoruce disconnected, select a new master resource and broadcast
//--------------------------
class MasterSelectionModule
{
public:

	explicit MasterSelectionModule( WinnerRelayApp& app);
	
	void AsyncAddResource(unsigned short raid_id, unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn);
	void AsyncRemoveResource(int conn_id);

private:

	void AddResource(unsigned short raid_id, unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn);
	void RemoveResource(int conn_id);

	WinnerRelayApp& app_;

	TSystem::TaskStrand strand_;

	// raid_id -> (master pid, vector(pid, pconn))
	// currently use the first resource in the vector as master 
	typedef std::vector<std::pair<unsigned short, std::shared_ptr<TSystem::communication::Connection>>> PidConnectionVectorType;
	std::map<unsigned short, std::pair<unsigned short, PidConnectionVectorType>> raid_map_;
};

#endif // MASTER_SELECTION_MODULE_H_