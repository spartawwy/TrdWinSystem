#ifndef QUOTATION_SERVER_APP_H_SDF55SDSF_
#define QUOTATION_SERVER_APP_H_SDF55SDSF_

#include <vector>

#include <TLib/tool/tsystem_server_appbase.h>

#include "WINNERLib/boost_locker.h"
#include "WINNERLib/winner_user_msg.pb.h"
#include "WINNERLib/quotation_msg.pb.h"
#include "data_base.h"
#include "exchange_calendar.h"
#include "quota_svr_common.h"

#define  GET_HOUR(a) ((a)/10000)
#define  GET_MINUTE(a) ((a)%10000/100)
#define  GET_SECOND(a) ((a)%100)

int bar_daystr_to_longday(const std::string &day_str);

using namespace TSystem;

class T_Fenbi
{
public:
    T_Fenbi():hhmmss(0), price(0.0){}
    T_Fenbi(int hms, float val):hhmmss(hms), price(val){}
    T_Fenbi(T_Fenbi && lh): hhmmss(lh.hhmmss), price(lh.price){}
    int hhmmss;
    float price;
};
// ( date, fenbis)
typedef std::unordered_map<int, std::shared_ptr<std::vector<T_Fenbi> > > TDayFenbi;
typedef std::unordered_map<std::string, TDayFenbi> TCodeDayFenbi;

class UserRequest; 
class ExchangeCalendar;
class QuotationServerApp : public ServerAppBase
{
public:

	QuotationServerApp(const std::string &name, const std::string &version);
	virtual ~QuotationServerApp();

	virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override 
	{
	};
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
		, const TError& te) override {};

	virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override;
	virtual void UpdateState() override;

	void Initiate();

private:

	void InitPython();
	void SetupServerSocket();
	void HandleUserRequest(std::shared_ptr<UserRequest>& req, std::shared_ptr<communication::Connection>& pconn);
	void HandleUserLogin(const UserRequest& req, const std::shared_ptr<communication::Connection>& pconn);

    void HandleQuotationRequest(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn);
    void _HandleQuotatoinFenbi(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn);
    void _HandleQuotatoinKbar(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection>& pconn);
    void _HandleQuotatoinKBarDay(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection> &pconn, const std::string &code, int beg_date, int end_date, QuotationFqType fqtye, bool is_index);
    void _HandleQuotatoinKBarWeek(std::shared_ptr<QuotationRequest>& req, std::shared_ptr<communication::Connection> &pconn, const std::string &code, int beg_date, int end_date, QuotationFqType fqtye, bool is_index);

	void SendRequestAck(int user_id, int req_id, RequestType type, const std::shared_ptr<TSystem::communication::Connection>& pconn);

    // date_beg : yyyyMMdd 
    std::vector<std::string> GetFenbi2File(const std::string &code, int date_beg, int date_end);
    // ret file names 
    std::vector<std::string> GetDayKbars2File(const std::string &code, int date_beg, int date_end
                        ,  QuotationFqType fq_type=QuotationFqType::FQ_BEFORE, bool is_index=false);
     
    std::vector<std::string> GetWeekKbars2File(const std::string &code, int date_beg, int date_end
                        ,  QuotationFqType fq_type=QuotationFqType::FQ_BEFORE, bool is_index=false);
    void ReadQuoteMessage(const std::vector<std::string> &ret_date_str_vector, const std::string &code, KLINE_TYPE k_type, int beg_date, int end_date, QuotationMessage &quotation_msg);

    // cur; pre_close;open;high;low;amount
    bool GetRealTimeK(const std::string &code, QuotationMessage::QuotationKbarMessage &kbar_msg, bool is_index=false);

private:

    void *PyFuncGetAllFill2File;
    void *PyFuncGetDayKbar2File;
    void *PyFuncGetWeekKbar2File;
    void *PyFuncGetRealTimeKbar;
    std::string stk_data_dir_;
    // ------------
    typedef std::unordered_map<int, std::shared_ptr<TaskStrand> > TConnidMapStrand;
    TConnidMapStrand conn_strands_;
    WRMutex  conn_strands_wr_mutex_; 
    // ------------
    ExchangeCalendar exchange_calendar_;

    DataBase data_base_;

    TCodeDayFenbi  code_fenbi_container_;
};

void Delay(unsigned short mseconds);
#endif // QUOTATION_SERVER_APP_H_SDF55SDSF_