#include "master_selection_module.h"

#include <TLib/core/tsystem_serialization.h>
#include <TLib/tool/tsystem_raid_master.pb.h>

#include "winner_relay_app.h"

using namespace TSystem;


MasterSelectionModule::MasterSelectionModule( WinnerRelayApp& app)
	: app_(app)
	, strand_(app_.task_pool())
	, raid_map_()
{}

void MasterSelectionModule::AddResource(unsigned short raid_id, unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	if( raid_id )
	{
		auto iter = raid_map_.find(raid_id);
		if( iter == raid_map_.end() )
		{
			iter = raid_map_.insert( std::make_pair(raid_id, std::make_pair(0, PidConnectionVectorType()) )).first;
			iter->second.second.reserve(4);
		}

		iter->second.second.push_back(std::make_pair(pid, std::move(pconn)));
		iter->second.first = iter->second.second.front().first;
		RaidMaster msg;
		msg.set_pid(iter->second.first);
		iter->second.second.back().second->AsyncSend(Encode(msg, app_.msg_system(), MessageHeader(0, app_.pid(), 0)));
	}else
	{
		pconn->AsyncSend(Encode(RaidMaster(), app_.msg_system(), MessageHeader(0, app_.pid(), 0)));
	}
}

void MasterSelectionModule::RemoveResource(int conn_id)
{
	std::for_each(begin(raid_map_), std::end(raid_map_),
		[this, conn_id](std::pair<const unsigned short, std::pair<unsigned short, PidConnectionVectorType>>& entry)
	{
		// for each raid_id -> (master pid, vecotr(pid, pconn)) map
		int remove = 0;
		std::remove_if(std::begin(entry.second.second), std::end(entry.second.second)
			, [this, conn_id, &remove, &entry](PidConnectionVectorType::const_reference pid_pconn)->bool
		{ 
			if( pid_pconn.second->connid() == conn_id )
			{
				++remove;
				return true;
			}else
				return false;
		} );
		
		if( remove )
		{
			entry.second.second.resize(entry.second.second.size()-1);
			
			if( entry.second.second.size() > 0 &&           // if still has resource in raid            
				entry.second.first != entry.second.second.front().first ) // master raid pid changed
			{
				entry.second.first = entry.second.second.front().first;
				RaidMaster msg;
				msg.set_pid(entry.second.first);
				communication::CodedDataPtr data = Encode(msg, app_.msg_system(), MessageHeader(0, app_.pid(), 0));
				for( auto& pid_pconn : entry.second.second )
					pid_pconn.second->AsyncSend(data);
			}
			return;
		}
	});
}

void MasterSelectionModule::AsyncAddResource(unsigned short raid_id, unsigned short pid, const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	strand_.PostTask( [this, raid_id, pid, pconn]()
	{
		this->AddResource(raid_id, pid, pconn);
	});
}

void MasterSelectionModule::AsyncRemoveResource(int conn_id)
{
	strand_.PostTask( [this, conn_id]()
	{
		this->RemoveResource(conn_id);
	});
}