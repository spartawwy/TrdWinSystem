#include "sys_common.h" 
 
#include <TLib/core/tsystem_time.h>
#include <chrono>
#include <thread>

#include <ctime>
 
#include <TLib/core/tsystem_utility_functions.h> 


void Delay(unsigned short mseconds)
{
	//TSystem::WaitFor([]()->bool { return false;}, mseconds); // only make effect to timer
	std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));

}
 
 
TypeMarket GetStockMarketType(const std::string &stock_code)
{
	return (stock_code.c_str()[0] == '6' ? TypeMarket::SH : TypeMarket::SZ);
}

//bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
//{
//	return lh.id < rh.id; 
//}

bool Equal(double lh, double rh)
{
	return fabs(lh-rh) < 0.0001;
}

//QTime Int2Qtime(int val)
//{
//	return QTime(val / 10000, (val % 10000) / 100, val % 100);
//}

bool IsStrAlpha(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalpha(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}

}

bool IsStrNum(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalnum(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}

}

std::string TagOfLog()
{
	return TSystem::utility::FormatStr("app_%d", TSystem::Today());
	//return  "OrderData_" + TSystem::Today()
}

std::string TagOfOrderLog()
{
	return TSystem::utility::FormatStr("OrderData_%d", TSystem::Today());
	//return  "OrderData_" + TSystem::Today()
}

std::tuple<int, std::string> CurrentDateTime()
{
	time_t rawtime;
	time(&rawtime);

	const int cst_buf_len = 256;
	char szContent[cst_buf_len] = {0};

	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);
	sprintf_s( szContent, cst_buf_len, "%02d:%02d:%02d"
		, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec ); 

	return std::make_tuple((timeinfo.tm_year + 1900) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday
		, std::string(szContent));

}

bool IsNowTradeTime()
{
	return true; //temp code
	static auto get_date = []()
	{
		time_t rawtime;
		struct tm timeinfo;
		time( &rawtime );
		localtime_s(&timeinfo, &rawtime ); // from 1900 year
		return timeinfo.tm_year * 10000 + timeinfo.tm_mon *100 + timeinfo.tm_mday;
	};


	static int week_day = 0;  
	static int ori_day = 0;
	static time_t sec_beg = 0;
	static time_t sec_rest_beg = 0;
	static time_t sec_rest_end = 0;
	static time_t sec_end = 0;

	time_t rawtime = 0;
	struct tm timeinfo;
	time( &rawtime );

	auto cur_day = get_date();
	if( ori_day != cur_day )
	{
		ori_day = cur_day;

		localtime_s(&timeinfo, &rawtime ); // from 1900 year
		week_day = timeinfo.tm_wday;

		struct tm tm_trade_beg;
		tm_trade_beg.tm_year = timeinfo.tm_year;
		tm_trade_beg.tm_mon = timeinfo.tm_mon;
		tm_trade_beg.tm_mday = timeinfo.tm_mday;
		tm_trade_beg.tm_hour = 9;
		tm_trade_beg.tm_min = 25;
		tm_trade_beg.tm_sec = 59;
		sec_beg = mktime(&tm_trade_beg);

		struct tm tm_rest_beg; 
		tm_rest_beg.tm_year = timeinfo.tm_year;
		tm_rest_beg.tm_mon = timeinfo.tm_mon;
		tm_rest_beg.tm_mday = timeinfo.tm_mday;
		tm_rest_beg.tm_hour = 11;
		tm_rest_beg.tm_min = 32;
		tm_rest_beg.tm_sec = 00;
		sec_rest_beg = mktime(&tm_rest_beg);

		struct tm tm_rest_end; 
		tm_rest_end.tm_year = timeinfo.tm_year;
		tm_rest_end.tm_mon = timeinfo.tm_mon;
		tm_rest_end.tm_mday = timeinfo.tm_mday;
		tm_rest_end.tm_hour = 12;
		tm_rest_end.tm_min = 58;
		tm_rest_end.tm_sec = 00;
		sec_rest_end = mktime(&tm_rest_end);

		struct tm tm_trade_end; 
		tm_trade_end.tm_year = timeinfo.tm_year;
		tm_trade_end.tm_mon = timeinfo.tm_mon;
		tm_trade_end.tm_mday = timeinfo.tm_mday;
		tm_trade_end.tm_hour = 15;
		tm_trade_end.tm_min = 32;
		tm_trade_end.tm_sec = 59;
		sec_end = mktime(&tm_trade_end);
	}

	if( week_day == 6 || week_day == 0 ) // sunday: 0, monday : 1 ...
		return false; 
	if( (rawtime >= sec_beg && rawtime <= sec_rest_beg) || (rawtime >= sec_rest_end && rawtime <= sec_end) )
		return true;
	else 
		return false;
}

std::chrono::system_clock::time_point Int2TimePoint(int val)
{
	// ndedt:  todo:
	 //return  TSystem::GetClock().Now();
	return std::chrono::system_clock::time_point();
}

 //bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
 //{
	// return lh.id < rh.id; 
 //}

 int GetBaseStocks(char* stocks[], int max_size)
 {





	 char *stock_codes[] = {"600000",
		 "600001",
		 "600002",
		 "600003",
		 "600004",
		 "600005",
		 "600006",
		 "600007",
		 "600008",
		 "600009",
		 "600010",
		 "600011",
		 "600012",
		 "600015",
		 "600016",
		 "600017",
		 "600018",
		 "600019",
		 "600020",
		 "600021",
		 "600022",
		 "600023",
		 "600026",
		 "600027",
		 "600028",
		 "600029",
		 "600030",
		 "600031",
		 "600033",
		 "600035",
		 "600036",
		 "600037",
		 "600038",
		 "600039",
		 "600048",
		 "600050",
		 "600051",
		 "600052",
		 "600053",
		 "600054",
		 "600055",
		 "600056",
		 "600057",
		 "600058",
		 "600059",
		 "600060",
		 "600061",
		 "600062",
		 "600063",
		 "600064",
		 "600065",
		 "600066",
		 "600067",
		 "600068",
		 "600069",
		 "600070",
		 "600071",
		 "600072",
		 "600073",
		 "600074",
		 "600075",
		 "600076",
		 "600077",
		 "600078",
		 "600079",
		 "600080",
		 "600081",
		 "600082",
		 "600083",
		 "600084",
		 "600085",
		 "600086",
		 "600087",
		 "600088",
		 "600089",
		 "600090",
		 "600091",
		 "600092",
		 "600093",
		 "600094",
		 "600095",
		 "600096",
		 "600097",
		 "600098",
		 "600099",
		 "600100",
		 "600101",
		 "600102",
		 "600103",
		 "600104",
		 "600105",
		 "600106",
		 "600107",
		 "600108",
		 "600109",
		 "600110",
		 "600111",
		 "600112",
		 "600113",
		 "600114",
		 "600115",
		 "600116",
		 "600117",
		 "600118",
		 "600119",
		 "600120",
		 "600121",
		 "600122",
		 "600123",
		 "600125",
		 "600126",
		 "600127",
		 "600128",
		 "600129",
		 "600130",
		 "600131",
		 "600132",
		 "600133",
		 "600135",
		 "600136",
		 "600137",
		 "600138",
		 "600139",
		 "600141",
		 "600143",
		 "600145",
		 "600146",
		 "600148",
		 "600149",
		 "600150",
		 "600151",
		 "600152",
		 "600153",
		 "600155",
		 "600156",
		 "600157",
		 "600158",
		 "600159",
		 "600160",
		 "600161",
		 "600162",
		 "600163",
		 "600165",
		 "600166",
		 "600167",
		 "600168",
		 "600169",
		 "600170",
		 "600171",
		 "600172",
		 "600173",
		 "600175",
		 "600176",
		 "600177",
		 "600178",
		 "600179",
		 "600180",
		 "600181",
		 "600182",
		 "600183",
		 "600184",
		 "600185",
		 "600186",
		 "600187",
		 "600188",
		 "600189",
		 "600190",
		 "600191",
		 "600192",
		 "600193",
		 "600195",
		 "600196",
		 "600197",
		 "600198",
		 "600199",
		 "600200",
		 "600201",
		 "600202",
		 "600203",
		 "600205",
		 "600206",
		 "600207",
		 "600208",
		 "600209",
		 "600210",
		 "600211",
		 "600212",
		 "600213",
		 "600215",
		 "600216",
		 "600217",
		 "600218",
		 "600219",
		 "600220",
		 "600221",
		 "600222",
		 "600223",
		 "600225",
		 "600226",
		 "600227",
		 "600228",
		 "600229",
		 "600230",
		 "600231",
		 "600232",
		 "600233",
		 "600234",
		 "600235",
		 "600236",
		 "600237",
		 "600238",
		 "600239",
		 "600240",
		 "600241",
		 "600242",
		 "600243",
		 "600246",
		 "600247",
		 "600248",
		 "600249",
		 "600250",
		 "600251",
		 "600252",
		 "600253",
		 "600255",
		 "600256",
		 "600257",
		 "600258",
		 "600259",
		 "600260",
		 "600261",
		 "600262",
		 "600263",
		 "600265",
		 "600266",
		 "600267",
		 "600268",
		 "600269",
		 "600270",
		 "600271",
		 "600272",
		 "600273",
		 "600275",
		 "600276",
		 "600277",
		 "600278",
		 "600279",
		 "600280",
		 "600281",
		 "600282",
		 "600283",
		 "600284",
		 "600285",
		 "600286",
		 "600287",
		 "600288",
		 "600289",
		 "600290",
		 "600291",
		 "600292",
		 "600293",
		 "600295",
		 "600296",
		 "600297",
		 "600298",
		 "600299",
		 "600300",
		 "600301",
		 "600302",
		 "600303",
		 "600305",
		 "600306",
		 "600307",
		 "600308",
		 "600309",
		 "600310",
		 "600311",
		 "600312",
		 "600313",
		 "600315",
		 "600316",
		 "600317",
		 "600318",
		 "600319",
		 "600320",
		 "600321",
		 "600322",
		 "600323",
		 "600325",
		 "600326",
		 "600327",
		 "600328",
		 "600329",
		 "600330",
		 "600331",
		 "600332",
		 "600333",
		 "600335",
		 "600336",
		 "600337",
		 "600338",
		 "600339",
		 "600340",
		 "600343",
		 "600345",
		 "600346",
		 "600348",
		 "600349",
		 "600350",
		 "600351",
		 "600352",
		 "600353",
		 "600354",
		 "600355",
		 "600356",
		 "600357",
		 "600358",
		 "600359",
		 "600360",
		 "600361",
		 "600362",
		 "600363",
		 "600365",
		 "600366",
		 "600367",
		 "600368",
		 "600369",
		 "600370",
		 "600371",
		 "600372",
		 "600373",
		 "600375",
		 "600376",
		 "600377",
		 "600378",
		 "600379",
		 "600380",
		 "600381",
		 "600382",
		 "600383",
		 "600385",
		 "600386",
		 "600387",
		 "600388",
		 "600389",
		 "600390",
		 "600391",
		 "600392",
		 "600393",
		 "600395",
		 "600396",
		 "600397",
		 "600398",
		 "600399",
		 "600400",
		 "600401",
		 "600403",
		 "600405",
		 "600406",
		 "600408",
		 "600409",
		 "600410",
		 "600415",
		 "600416",
		 "600418",
		 "600419",
		 "600420",
		 "600421",
		 "600422",
		 "600423",
		 "600425",
		 "600426",
		 "600428",
		 "600429",
		 "600432",
		 "600433",
		 "600435",
		 "600436",
		 "600438",
		 "600439",
		 "600444",
		 "600446",
		 "600448",
		 "600449",
		 "600452",
		 "600455",
		 "600456",
		 "600458",
		 "600459",
		 "600460",
		 "600461",
		 "600462",
		 "600463",
		 "600466",
		 "600467",
		 "600468",
		 "600469",
		 "600470",
		 "600472",
		 "600475",
		 "600476",
		 "600477",
		 "600478",
		 "600479",
		 "600480",
		 "600481",
		 "600482",
		 "600483",
		 "600485",
		 "600486",
		 "600487",
		 "600488",
		 "600489",
		 "600490",
		 "600491",
		 "600493",
		 "600495",
		 "600496",
		 "600497",
		 "600498",
		 "600499",
		 "600500",
		 "600501",
		 "600502",
		 "600503",
		 "600505",
		 "600506",
		 "600507",
		 "600508",
		 "600509",
		 "600510",
		 "600511",
		 "600512",
		 "600513",
		 "600515",
		 "600516",
		 "600517",
		 "600518",
		 "600519",
		 "600520",
		 "600521",
		 "600522",
		 "600523",
		 "600525",
		 "600526",
		 "600527",
		 "600528",
		 "600529",
		 "600530",
		 "600531",
		 "600532",
		 "600533",
		 "600535",
		 "600536",
		 "600537",
		 "600538",
		 "600539",
		 "600540",
		 "600543",
		 "600545",
		 "600546",
		 "600547",
		 "600548",
		 "600549",
		 "600550",
		 "600551",
		 "600552",
		 "600553",
		 "600555",
		 "600556",
		 "600557",
		 "600558",
		 "600559",
		 "600560",
		 "600561",
		 "600562",
		 "600563",
		 "600565",
		 "600566",
		 "600567",
		 "600568",
		 "600569",
		 "600570",
		 "600571",
		 "600572",
		 "600573",
		 "600575",
		 "600576",
		 "600577",
		 "600578",
		 "600579",
		 "600580",
		 "600581",
		 "600582",
		 "600583",
		 "600584",
		 "600585",
		 "600586",
		 "600587",
		 "600588",
		 "600589",
		 "600590",
		 "600591",
		 "600592",
		 "600593",
		 "600594",
		 "600595",
		 "600596",
		 "600597",
		 "600598",
		 "600599",
		 "600600",
		 "600601",
		 "600602",
		 "600603",
		 "600604",
		 "600605",
		 "600606",
		 "600607",
		 "600608",
		 "600609",
		 "600610",
		 "600611",
		 "600612",
		 "600613",
		 "600614",
		 "600615",
		 "600616",
		 "600617",
		 "600618",
		 "600619",
		 "600620",
		 "600621",
		 "600622",
		 "600623",
		 "600624",
		 "600625",
		 "600626",
		 "600627",
		 "600628",
		 "600629",
		 "600630"};

		int i = 0;
		for( ; i < sizeof(stock_codes)/sizeof(stock_codes[0]) && i < max_size; ++i )
		{
			stocks[i] = stock_codes[i];
		}
		return i;
 }