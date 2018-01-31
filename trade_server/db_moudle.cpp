
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
	: app_(app)
	, db_conn_(nullptr)
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
	LoadAllBrokers();
	LoadAllUsers();
	LoadAllAccounts();

	if( !utility::ExistTable("stocks", *db_conn_) )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
			, "DBMoudle::Init"
			, "can't find table stocks");

	std::string sql = "SELECT code, broker_tsk_tb_str FROM Stocks WHERE code like '0%' ORDER BY code ";
	db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
	{ 
		 auto obj = std::make_shared<T_CodeBrokerTaskTables>(*vals, std::move(TSystem::utility::split(*(vals+1), ";")));
		 app_->code_table_container_beg_0_.push_back(std::move(obj));
		 return 0;
	});

	sql = "SELECT code, broker_tsk_tb_str FROM Stocks WHERE code like '3%' ORDER BY code ";
	db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
	{ 
		auto obj = std::make_shared<T_CodeBrokerTaskTables>(*vals, std::move(TSystem::utility::split(*(vals+1), ";")));
		app_->code_table_container_beg_3_.push_back(std::move(obj));
		return 0;
	});
	sql = "SELECT code, broker_tsk_tb_str FROM Stocks WHERE code like '6%' ORDER BY code ";
	db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
	{ 
		auto obj = std::make_shared<T_CodeBrokerTaskTables>(*vals, std::move(TSystem::utility::split(*(vals+1), ";")));
		app_->code_table_container_beg_6_.push_back(std::move(obj));
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

void DBMoudle::LoadAllBrokers()
{
	if( !db_conn_ )
		Open(db_conn_);
	//-----------load all brokers-----------------
	if( !utility::ExistTable("BrokerInfo", *db_conn_) )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
		, "DBMoudle::Init"
		, "can't find table BrokerInfo");
	std::string sql = "SELECT id, ip, port, type, remark, com_ver FROM BrokerInfo ORDER BY type ";

	db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
	{
		auto p_broker_info = std::make_shared<T_BrokerInfo>();
		try
		{
			p_broker_info->id = boost::lexical_cast<int>(*vals); 
			p_broker_info->port = boost::lexical_cast<int>(*(vals + 2)); 
			p_broker_info->type = boost::lexical_cast<int>(*(vals + 3)); 
		}catch(boost::exception &)
		{
			std::cout << "DBMoudle::LoadAllBrokers lexical_cast exception" << std::endl;
			app_->local_logger().LogLocal("DBMoudle::LoadAllBrokers lexical_cast exception");
			return 0;
		}
		p_broker_info->ip = *(vals + 1);
		p_broker_info->remark  = *(vals + 4); 
		p_broker_info->com_ver  = *(vals + 5); 
		 
		//auto p_broker_info = std::make_shared<T_BrokerInfo>(id, ip, port, type, remark, com_ver); // cpp12 not support

		this->app_->id_brokers_.emplace(std::make_pair(p_broker_info->id, std::move(p_broker_info)));
		return 0;
	});
}

void DBMoudle::LoadAllUsers()
{
	// todo:
}

void DBMoudle::LoadAllAccounts()
{
	// todo:
}