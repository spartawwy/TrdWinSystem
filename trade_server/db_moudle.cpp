
#if 0
CREATE TABLE AccountInfo(id INTEGER, account_no TEXT, trade_account_no TEXT, trade_pwd  TEXT, comm_pwd TEXT
, broker_id INTEGER, department_id text, remark TEXT, PRIMARY KEY(id));

/* id use telno */
CREATE TABLE UserInfo(id text 
	, level INTEGER
	, name TEXT UNIQUE
	, nick_name TEXT
	, password TEXT
	, account_id INTEGER
	, remark TEXT, PRIMARY KEY(id, account_id) );

//table sh stocks (begin wich 6)
//	stock (key) | broker_task_table_name0; broker_task_table_name1; | task_id0; task_id1|
CREATE TABLE Stocks( code TEXT NOT NULL
					, broker_tsk_tb_str TEXT
					, PRIMARY KEY(code));
 
CREATE TABLE BrokerTask_FANG_ZHENG(task_id TEXT
				 , task_type INTEGER
				 , stock TEXT
				 , user_id INTEGER
				 , account_id INTEGER
				 , PRIMARY KEY(task_id));
//every account_id has a strand
CREATE TABLE BrokerTask_FANG_ZHENG(task_id TEXT
								   , task_type INTEGER
								   , stock TEXT NOT NULL
								   , user_id INTEGER
								   , account_id INTEGER
								   , state INTEGER
								   , PRIMARY KEY(task_id));
CREATE TABLE BrokerTask_PING_AN(task_id TEXT
								, task_type INTEGER
								, stock TEXT NOT NULL
								, user_id INTEGER
								, account_id INTEGER
								, state INTEGER
								, PRIMARY KEY(task_id));
CREATE TABLE BrokerTask_ZHONGXIN(task_id TEXT
								 , task_type INTEGER
								 , stock TEXT NOT NULL
								 , user_id INTEGER
								 , account_id INTEGER
								 , state INTEGER
								 , PRIMARY KEY(task_id));
CREATE TABLE BrokerTask_ZHONGYGJ(task_id TEXT
								 , task_type INTEGER
								 , stock TEXT NOT NULL
								 , user_id INTEGER
								 , account_id INTEGER
								 , state INTEGER
								 , PRIMARY KEY(task_id));
// not support 
// CREATE TABLE BrokerTask_YINGHE_ZQ(task_id TEXT
#endif 

#include "db_moudle.h"

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <memory>

#include <SQLite/sqlite_connection.h>
#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_sqlite_functions.h>

#include "trade_server_app.h"

DBMoudle::DBMoudle(TradeServerApp *app)
	:db_conn_(nullptr)
{

}

DBMoudle::~DBMoudle()
{

}

void DBMoudle::Init()
{ 
	// SELECT code, broker_tsk_tb_str FROM stocks ORDER BY code;
	if( !db_conn_ )
		Open(db_conn_);

	if( !utility::ExistTable("stocks", *db_conn_) )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
		, "DBMoudle::Init"
		, "can't find table accountInfo");

	 
	std::list<T_CodeBrokerTables> stocks_broker_table_list;
	//std::string pre_stock;
	std::string sql = "SELECT code, broker_tsk_tb_str FROM stocks ORDER BY code ";
	db_conn_->ExecuteSQL(sql.c_str(),[&stocks_broker_table_list, this](int num_cols, char** vals, char** names)->int
	{
		 /*if( pre_stock != *vals )
		 {
			 stocks_broker_table_list.emplace_back(*vals);
			 pre_stock = *vals;
		 }
		 auto iter = stocks_broker_table_list.rbegin();*/
		 auto table_array = TSystem::utility::split(*(vals+1), ";");
		 std::string stock = *vals;
		 stocks_broker_table_list.emplace_back(std::move(stock), std::move(table_array));
		 return 0;
	});

}

void DBMoudle::Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn)
{
	db_conn = std::make_shared<SQLite::SQLiteConnection>();

	std::string db_file = "./xswp.kd";

	if( db_conn->Open(db_file.c_str(), SQLite::SQLiteConnection::OpenMode::READ_WRITE) != SQLite::SQLiteCode::OK )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
		, "DBMoudle::Open"
		, "can't open database: " + db_file);

}