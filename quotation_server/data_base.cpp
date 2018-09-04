#include "data_base.h"

#include <boost/lexical_cast.hpp>

#include <SQLite/sqlite_connection.h>
#include <TLib/core/tsystem_sqlite_functions.h>

#include "exchange_calendar.h"

using namespace TSystem;

static std::string exchane_db_file_path = "./exchbase.kd";

DataBase::DataBase()
{

}

void DataBase::Init()
{

}

void DataBase::Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn, const std::string db_file)
{
    db_conn = std::make_shared<SQLite::SQLiteConnection>();

    //std::string db_file = "./pzwj.kd";
    if( db_conn->Open(db_file.c_str(), SQLite::SQLiteConnection::OpenMode::READ_WRITE) != SQLite::SQLiteCode::OK )
        throw "can't open database: " + db_file;
}

void DataBase::LoadTradeDate(void *exchange_calendar)
{
    assert(exchange_calendar);
    if( !exchange_db_conn_ )
       Open(exchange_db_conn_, exchane_db_file_path);

    if( !utility::ExistTable("ExchangeDate", *exchange_db_conn_) )
        throw "DBMoudle::LoadTradeDate can't find table ExchangeDate"; 
     
    std::string sql = "SELECT date FROM ExchangeDate WHERE is_tradeday = 1 ORDER BY date ";
    int num = 0;
    ((ExchangeCalendar*)exchange_calendar)->min_trade_date_ = 0;
    exchange_db_conn_->ExecuteSQL(sql.c_str(),[&num, &exchange_calendar, this](int num_cols, char** vals, char** names)->int
    { 
        try
        { 
            ++num;
            int date =  boost::lexical_cast<int>(*(vals)); 
            ((ExchangeCalendar*)exchange_calendar)->trade_dates_->insert(std::make_pair(date, true));

            if( ((ExchangeCalendar*)exchange_calendar)->min_trade_date_ == 0 )
                ((ExchangeCalendar*)exchange_calendar)->min_trade_date_ = date;

            ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ = date;
         }catch(boost::exception& )
        {
            return 0;
        } 
        return 0;
    }); 
}