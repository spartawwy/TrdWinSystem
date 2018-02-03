
#if 0
CREATE TABLE BrokerInfo (id INTEGER, ip TEXT, port INTEGER, type INTEGER, remark TEXT, com_ver text, PRIMARY KEY(id));

CREATE TABLE BrokerTypeTaskTable( type INTEGER, table_name TEXT PRIMARY KEY(type));
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
//CREATE TABLE BrokerTask_FANG_ZHENG(task_id TEXT
//								   , task_type INTEGER
//								   , stock TEXT NOT NULL
//								   , user_id INTEGER
//								   , account_id INTEGER
//								   , state INTEGER
//								   , PRIMARY KEY(task_id));

// use different broker task table is to avoid lock,cause there may be thounds task ajust in a timepoint
CREATE TABLE BrokerTask_FANG_ZHENG(task_id TEXT, task_type INTEGER,
								   stock TEXT NOT NULL, user_id INTEGER,
								   account_id INTEGER, state INTEGER,
								   alert_price  DOUBLE, back_alert_trigger  BOOL,
								   rebounce  DOUBLE, continue_second   INTEGER,
								   step  DOUBLE, quantity  INTEGER,
								   target_price_level INTEGER, start_time   INTEGER,
								   end_time     INTEGER, is_loop      BOOL,
								   bs_times     INTEGER, assistant_field  TEXT,
								   PRIMARY KEY(task_id));


CREATE TABLE BrokerTask_PING_AN(task_id TEXT, task_type INTEGER,
								stock TEXT NOT NULL, user_id INTEGER,
								account_id INTEGER, state INTEGER,
								alert_price  DOUBLE, back_alert_trigger  BOOL,
								rebounce  DOUBLE, continue_second   INTEGER,
								step  DOUBLE, quantity  INTEGER,
								target_price_level INTEGER, start_time   INTEGER,
								end_time     INTEGER, is_loop      BOOL,
								bs_times     INTEGER, assistant_field  TEXT,
								PRIMARY KEY(task_id));

CREATE TABLE BrokerTask_ZHONGXIN(task_id TEXT, task_type INTEGER,
								 stock TEXT NOT NULL, user_id INTEGER,
								 account_id INTEGER, state INTEGER,
								 alert_price  DOUBLE, back_alert_trigger  BOOL,
								 rebounce  DOUBLE, continue_second   INTEGER,
								 step  DOUBLE, quantity  INTEGER,
								 target_price_level INTEGER, start_time   INTEGER,
								 end_time     INTEGER, is_loop      BOOL,
								 bs_times     INTEGER, assistant_field  TEXT,
								 PRIMARY KEY(task_id));

CREATE TABLE BrokerTask_ZHONGYGJ(task_id TEXT, task_type INTEGER,
								 stock TEXT NOT NULL, user_id INTEGER,
								 account_id INTEGER, state INTEGER,
								 alert_price  DOUBLE, back_alert_trigger  BOOL,
								 rebounce  DOUBLE, continue_second   INTEGER,
								 step  DOUBLE, quantity  INTEGER,
								 target_price_level INTEGER, start_time   INTEGER,
								 end_time     INTEGER, is_loop      BOOL,
								 bs_times     INTEGER, assistant_field  TEXT,
								 PRIMARY KEY(task_id));        

CREATE TABLE EqualSectionTask(task_id TEXT,
							  raise_percent DOUBLE, fall_percent DOUBLE,
							  raise_infection DOUBLE, fall_infection DOUBLE,
							  multi_qty INTEGER,
							  max_trig_price DOUBLE, min_trig_price DOUBLE,
							  is_original BOOL,
							  max_position INTEGER, min_position INTEGER,
							  PRIMARY KEY(task_id));
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
	LoadAllBrokerTypeTaskTable();
	LoadAllBrokers();
	LoadAllUsers();
	LoadAllAccounts();
	LoadAllTasks();

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

void DBMoudle::LoadAllBrokerTypeTaskTable()
{
	if( !db_conn_ )
		Open(db_conn_);
	//-----------load all brokers-----------------
	if( !utility::ExistTable("BrokerTypeTaskTable", *db_conn_) )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
		, "DBMoudle::Init"
		, "can't find table BrokerTypeTaskTable");
	std::string sql = "SELECT type, table_name FROM BrokerTypeTaskTable ORDER BY type ";
	db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
	{
		int type = boost::lexical_cast<int>(*vals); 
		std::string table_name = *(vals + 1);
		this->app_->type_broker_task_tablename_.insert(std::make_pair(type, std::move(table_name)));
		return 0;
	});
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

void DBMoudle::LoadAllTasks()
{ 
	if( !db_conn_ )
		Open(db_conn_);
	//-----------load all tasks-----------------
	/*
	 
	*/
	std::for_each( std::begin(this->app_->type_broker_task_tablename_), std::end(this->app_->type_broker_task_tablename_)
		,[this](std::unordered_map<int, std::string>::reference entry)
	{
		const int broker_type = entry.first;
		if( !utility::ExistTable(entry.second, *db_conn_) )
			ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
			, "DBMoudle::Init"
			, utility::FormatStr("can't find table %s", entry.second.c_str()) );

		auto accountid_idtasks = std::make_shared<std::unordered_map<int, T_Id_TaskInfo_Map> >(1024);
		auto item = this->app_->broker_account_taskid_info_.insert( std::make_pair(broker_type, std::move(accountid_idtasks)) ).first;
		//auto p_accountid_idtasks = item->second;
		std::shared_ptr<std::unordered_map<int, T_Id_TaskInfo_Map> >& ref_accountid_idtasks = item->second;
		// notice account_id exist in AccountInfo and stock exist in Stock
		std::string sql = utility::FormatStr("SELECT task_id, task_type, stock, user_id, account_id " 
			", state, alert_price, back_alert_trigger, rebounce, continue_second "
			", step, quantity, target_price_level, start_time, end_time "
			", is_loop, bs_times, assistant_field "
			" FROM %s ORDER BY account_id, stock ", entry.second.c_str());
		this->db_conn_->ExecuteSQL(sql.c_str(),[&, this](int num_cols, char** vals, char** names)->int
		{
			auto task_id = boost::lexical_cast<int64_t>(*(vals)); 
			auto account_id = boost::lexical_cast<int>(*(vals + 4)); 
			auto id_task_map_iter = ref_accountid_idtasks->find(account_id);
			if( id_task_map_iter == ref_accountid_idtasks->end() )
			{
				id_task_map_iter = ref_accountid_idtasks->insert( std::make_pair(account_id, T_Id_TaskInfo_Map(cst_max_task_per_account)) ).first;
			}
			T_Id_TaskInfo_Map &id_task_map = id_task_map_iter->second;
			T_TaskInformation task_info;
			// todo: fill task_info
			id_task_map.insert( std::make_pair(task_id, task_info) ); // ndedt: to use move
			return 0;
		});
	});
	
}