#ifndef SYS_COMMON_SDFSD3DD_H_
#define SYS_COMMON_SDFSD3DD_H_
  
#include "targetver.h"
#include <tchar.h> 

#include <cassert>
#include <stdio.h>
#include <stdint.h>

#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <memory>

#ifndef IN
#define  IN
#define  OUT
#define  INOUT
#endif

#define EQSEC_MAX_POSITION 9999900
#define EQSEC_MIN_POSITION 0

#define EQSEC_MAX_STOP_PRICE 999.00
#define EQSEC_MIN_CLEAR_PRICE 0.0

#define MAX_STOCKS  400

#if 0
std::vector<std::string> split(const std::string& line, const std::string& seperator = " ", 
							   const std::string& quotation = "\"");

std::string&   replace_all(std::string&   str, const   std::string&   old_value, const   std::string&   new_value);
std::string&   replace_all_distinct(std::string&   str, const   std::string&   old_value, const  std::string&   new_value);
#endif

const std::string cst_sh_index = "000001";
const char cst_sh_index_name[]  = "上证指数";

const std::string cst_sz_compre_index = "399001";
const char cst_sz_compre_index_name[]  = "深圳成指";

const std::string cst_entrepren_plate_index = "399006";
const char cst_entrepren_plate_index_name[]  = "创业板指数";

const std::string cst_entreplate_compre_index = "399102"; //创业板综合
const char cst_entreplate_compre_index_name[]  = "创业板综指";

const int cst_max_task_per_account = 30;

enum class TypeMarket : char
{
	SZ = 0,
	SH
};

enum class TypeOrderCategory : char
{
	BUY = 0,
	SELL = 1,
	RQ_BUY,
	RQ_SELL,
	BQ_HQ,
	SQ_HQ,
	CQ_HQ,
};
// 0资金  1股份   2当日委托  3当日成交     4可撤单   5股东代码  6融资余额   7融券余额  8可融证券</param>
enum class TypeQueryCategory : char
{
	CAPITAL = 0,
	STOCK = 1,
	TODAY_DELEGATE = 2,
	TODAY_FILL = 3,
	CAN_RECALL_ORDER,
	SHARED_HOLDER_CODE = 5,
	RZYE,
	RQYE,
	KRZQ,
};

enum class TypeUserLevel : char
{
	NORMAL = 1,
};

enum class TypeBroker : char
{
	FANG_ZHENG = 1,    //方正
	PING_AN,           //平安
	ZHONGXIN,          //中信
	ZHONGYGJ,          //中银国际

	//YINGHE_ZQ,         //银河证券
};

enum class TypeTask : char
{
	INFLECTION_BUY = 0, 
	BREAKUP_BUY,
	BATCHES_BUY,

	INFLECTION_SELL,  //3
	BREAK_SELL,
	FOLLOW_SELL,
	BATCHES_SELL,

	EQUAL_SECTION,
	INDEX_RISKMAN, // 8
};

enum class TypeQuoteLevel : char
{
	PRICE_CUR = 0,
	PRICE_BUYSELL_1,
	PRICE_BUYSELL_2,
	PRICE_BUYSELL_3,
	PRICE_BUYSELL_4,
	PRICE_BUYSELL_5,
};

enum class TaskStatChangeType : char
{
	CUR_PRICE_CHANGE = 0,
	CUR_STATE_CHANGE,
};

// in database, STOP and WAITING is enough
enum class TaskCurrentState : char
{
	STOP = 0,  // not to run
	WAITTING, // means: not in tasktime, hasn't been register 

	// follow state is used for task which is registered
	STARTING,
	RUNNING,
	REST,     // exchange is in rest time
	EXCEPT,   // exception : in running time but not in running state
};


enum class TypeEqSection : char
{
	STOP = 0,
	SELL,
	NOOP,
	BUY,
	CLEAR,
};

struct T_SectionAutom
{
	TypeEqSection  section_type;
	double represent_price;
	T_SectionAutom() : section_type(TypeEqSection::NOOP), represent_price(0.0){}
	T_SectionAutom(TypeEqSection type, double resp_price) : section_type(type), represent_price(resp_price){}
};

struct T_AccountData
{
	char shared_holder_code[64]; 
	char name[64];
	TypeMarket type;
	char capital_code[64]; 
	char seat_code[64];
	char rzrq_tag[64];  //融资融券 标识
};

struct T_BrokerInfo
{ 
	int id;
	std::string ip;
	int port;
	int type;
	std::string remark;
	std::string com_ver;
	T_BrokerInfo() : id(0), ip(), port(0), type(0), remark(), com_ver(){}
	/*T_BrokerInfo(int i_d, const std::string& i_p, int para_port, int tp, const std::string& para_remake
	, const std::string& para_com_ver)
	: id(i_d), ip(i_p), port(para_port), type(tp), remark(para_remake), com_ver(para_com_ver){}*/
};

struct T_UserInfo
{ // PRIMARY KEY(id, account_id)
	std::string id;
	int level;
	std::string name;
	std::string nick_name;
	std::string password;
	int account_id;
	std::string remark;
	T_UserInfo() : id(0), level(0), account_id(0){}

	T_UserInfo &operator = (const T_UserInfo&lh)
	{
		id = lh.id; level = lh.level; name = lh.name; nick_name = lh.nick_name;
		password = lh.password; account_id = lh.account_id; remark = lh.remark;  
		return *this;
	}
};

struct T_AccountInfo
{
	int id;
	std::string account_no;
	std::string trade_acc_no;
	std::string trade_pwd;
	std::string comm_pwd;
	int broker_id; 
	std::string department_id; 
	std::string remark;
	T_AccountInfo() : id(-1), broker_id(-1) {}
};

struct T_PositionData
{
	std::string code;
	std::string pinyin;
	int total;
	int avaliable;
	double cost;
	double value;
	double profit;
	double profit_percent;
	T_PositionData() : code(), pinyin(), total(0), avaliable(0), cost(0.0), value(0.0), profit(0.0), profit_percent(0.0){}
	T_PositionData(const T_PositionData &lh) : code(lh.code), pinyin(lh.pinyin), total(lh.total),avaliable(lh.avaliable), cost(lh.cost), value(lh.value), profit(lh.profit), profit_percent(lh.profit_percent){}
	T_PositionData(const T_PositionData &&lh) : code(std::move(lh.code)), pinyin(std::move(lh.pinyin)), total(lh.total),avaliable(lh.avaliable), cost(lh.cost), value(lh.value), profit(lh.profit), profit_percent(lh.profit_percent){}
};

class Buffer
{
public:
	// notice size + 1 == 1024 is suitable for avoid memory fragment
	explicit Buffer(unsigned int size=63) : size_(size), p_data_(nullptr) 
	{
		p_data_ = new char[size + 1];
		memset(p_data_, 0, size + 1); 
	}
	explicit Buffer(const char* p_str, unsigned int size) 
	{
		assert(p_str);
		p_data_ = new char[size + 1];
		size_ = size;
		memcpy(p_data_, p_str, size);
		p_data_[size] = '\0';
	} 

	~Buffer(){ if( p_data_ ) delete[] p_data_; p_data_ = nullptr;};

	char * data() { return p_data_; }
	const char *c_data() const {return p_data_;}
	unsigned int size() const {return size_;}
	void reset() { memset(p_data_, 0, size_ + 1); }

private:

	Buffer(Buffer&);
	Buffer& operator = (Buffer&);

	unsigned int size_;
	char *p_data_;
};

class  QuotesData
{
public:
	int64_t time_stamp;
	double cur_price;
	double active_degree;
	double price_b_1;
	double price_s_1;
	double price_b_2;
	double price_s_2;
	double price_b_3;
	double price_s_3;
	double price_b_4;
	double price_s_4;
	double price_b_5;
	double price_s_5;
	QuotesData() 
		: time_stamp(0)
		, cur_price(0.0)  
		, active_degree(0.0)
		, price_b_1(0.0)
		, price_s_1(0.0)
		, price_b_2(0.0)
		, price_s_2(0.0)
		, price_b_3(0.0)
		, price_s_3(0.0)
		, price_b_4(0.0)
		, price_s_4(0.0)
		, price_b_5(0.0)
		, price_s_5(0.0)
	{
	}
};
  
struct T_SectionTask
{ 
	double raise_percent;
	double fall_percent;
	double raise_infection;
	double fall_infection; 
	int    multi_qty;
	int    max_position;
	int    min_position;
	double max_trig_price;
	double min_trig_price;
	std::string sections_info; //format: section0_type$section0_price#section1_type$section1_price
	bool is_original;
	T_SectionTask():raise_percent(0.0), fall_percent(0.0), raise_infection(0.0), fall_infection(0.0)
		, multi_qty(0),max_trig_price(EQSEC_MAX_STOP_PRICE), min_trig_price(EQSEC_MIN_CLEAR_PRICE), sections_info(), is_original(true), max_position(EQSEC_MAX_POSITION), min_position(EQSEC_MIN_POSITION){}
	T_SectionTask(const T_SectionTask&lh): raise_percent(lh.raise_percent), fall_percent(lh.fall_percent), raise_infection(lh.raise_infection), fall_infection(lh.fall_infection)
		, multi_qty(lh.multi_qty),max_trig_price(lh.max_trig_price), min_trig_price(lh.min_trig_price), sections_info(lh.sections_info), is_original(lh.is_original), max_position(lh.max_position), min_position(lh.min_position) {}
};

enum class TindexTaskType : char
{
	ALERT,
	RELSTOCK,
	CLEAR,
};

struct T_IndexRelateTask
{ 
	TindexTaskType rel_type; // releate type
	bool is_down_trigger;
	std::string stock_code;  // sub related stock 
	bool is_buy; // if false means sell
	T_IndexRelateTask() : rel_type(TindexTaskType::ALERT), is_down_trigger(true), stock_code(), is_buy(false){}
	T_IndexRelateTask(const T_IndexRelateTask &lh) : rel_type(lh.rel_type), is_down_trigger(lh.is_down_trigger), stock_code(lh.stock_code), is_buy(lh.is_buy){}
};

struct T_TaskInformation
{
	unsigned int id;
	TypeTask  type;
	std::string stock;        // TEXT not null, alse can be index code--when type is INDEX_RISKMAN 
	int user_id;
	int account_id;
	TaskCurrentState state;    

	double alert_price;       // DOUBLE, 
	bool back_alert_trigger;  // 是否重回则触发
	double rebounce;          // 反弹或回撤 百分值
	int continue_second;      // 0-- not open >0 -- open
	double step;              // 阶梯幅度(以后每涨,以后每降) 百分值
	unsigned int quantity;    // 买入数量, 每次数量
	T_SectionTask secton_task;
	T_IndexRelateTask index_rel_task;
	int target_price_level;   // 0--即时价  1--买一和卖一 2--买二和卖二 3--买三和卖三 ...
	int start_time; 
	int end_time;
	bool is_loop;
	int  bs_times;            // 卖出次数, used by batches sell batches buy
	std::string  assistant_field;       // used by batches sell; batches buy
	T_TaskInformation() : id(0), type(TypeTask::BREAK_SELL), stock(), user_id(0), account_id(0), state(TaskCurrentState::STOP), alert_price(0)
		,back_alert_trigger(0), rebounce(0), continue_second(0), step(0.0), quantity(0), target_price_level(0), start_time(0), end_time(0), is_loop(0), bs_times(0), assistant_field() {} 

	T_TaskInformation(const T_TaskInformation& lh) : id(lh.id), type(lh.type), stock(lh.stock), user_id(lh.user_id), account_id(lh.account_id), state(lh.state), alert_price(lh.alert_price), back_alert_trigger(lh.back_alert_trigger)
		, rebounce(lh.rebounce), continue_second(lh.continue_second), step(lh.step), quantity(lh.quantity), secton_task(lh.secton_task), index_rel_task(lh.index_rel_task)
		, target_price_level(lh.target_price_level), start_time(lh.start_time), end_time(lh.end_time), is_loop(lh.is_loop), bs_times(lh.bs_times)
		, assistant_field(lh.assistant_field) { }
#if 0
	T_TaskInformation(T_TaskInformation&& lh) : id(lh.id), type(lh.type), stock(lh.stock), user_id(lh.user_id), account_id(lh.account_id), state(lh.state), alert_price(lh.alert_price), back_alert_trigger(lh.back_alert_trigger)
		, rebounce(lh.rebounce), continue_second(lh.continue_second), step(lh.step), quantity(lh.quantity), secton_task(lh.secton_task), index_rel_task(lh.index_rel_task)
		, target_price_level(lh.target_price_level), start_time(lh.start_time), end_time(lh.end_time), is_loop(lh.is_loop), bs_times(lh.bs_times)
		, assistant_field(lh.assistant_field) { }
#endif
};

class T_CodeBrokerTaskTables
{
public:
	T_CodeBrokerTaskTables(const std::string &stock) : stock_(stock){}
	T_CodeBrokerTaskTables(std::string &&stock, std::vector<std::string> &&broker_tables) : stock_(std::move(stock)), broker_tables_(std::move(broker_tables)){}
	std::string stock_;
	std::vector<std::string> broker_tables_;
};

//typedef std::unordered_map<std::string, std::shared_ptr<std::vector<std::string> > > T_CodeMapTableList;

typedef std::unordered_map<int64_t, T_TaskInformation> T_Id_TaskInfo_Map;

void Delay(unsigned short mseconds);

TypeMarket GetStockMarketType(const std::string &stock_code);

std::chrono::system_clock::time_point Int2TimePoint(int val);

//bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh);

bool Equal(double lh, double rh);

int GetBaseStocks(char* stocks[MAX_STOCKS], int max_size);

#endif

