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
	class T_CodeBrokerTables
	{
	public:
		T_CodeBrokerTables(const std::string &stock) : stock_(stock){}
		T_CodeBrokerTables(std::string &&stock, std::vector<std::string> &&broker_tables) : stock_(std::move(stock)), broker_tables_(std::move(broker_tables)){}
		std::string stock_;
		std::vector<std::string> broker_tables_;
	};

	DBMoudle(TradeServerApp *app);
	~DBMoudle();

	void Init();

private:

	void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn);

private:

	std::shared_ptr<SQLite::SQLiteConnection> db_conn_;
	int max_accoun_id_;
};

#endif //DBMOULDE_H_DSFWDSF_