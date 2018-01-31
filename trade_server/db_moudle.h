#ifndef DBMOULDE_H_DSFWDSF_
#define DBMOULDE_H_DSFWDSF_

#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <string>
#include <atomic>
#include <unordered_map>

#include "sys_common.h"

namespace SQLite
{
	class SQLiteConnection;
}

namespace TSystem
{
	class TaskStrand;
}
//namespace TSystem
//{
//    class LocalLogger;
//} 
 
class  TradeServerApp;
class DBMoudle
{
public:  

	DBMoudle(TradeServerApp *app);
	~DBMoudle();

	void Init();


private:

	void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn);
	void LoadAllBrokers();
	void LoadAllUsers();
	void LoadAllAccounts();

private:

	TradeServerApp *app_;
	std::shared_ptr<SQLite::SQLiteConnection> db_conn_;
	int max_accoun_id_; 
};

#endif //DBMOULDE_H_DSFWDSF_