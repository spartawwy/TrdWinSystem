#ifndef DATA_BASE_SDF32_H_
#define DATA_BASE_SDF32_H_

#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <string>
#include <atomic>
#include <unordered_map>

namespace SQLite
{
    class SQLiteConnection;
}

//namespace TSystem
//{
//    class TaskStrand;
//}

typedef std::unordered_map<int, bool> T_DateMapIsopen ;

class DataBase
{
public:

    DataBase();
    ~DataBase(){};

    void Init();
     
    void LoadTradeDate(void *exchange_calendar);

private:

    void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn, const std::string db_file);

    std::shared_ptr<SQLite::SQLiteConnection>  exchange_db_conn_;

    //std::string exchane_db_file_path_;
}; // DataBase


#endif  // DATA_BASE_SDF32_H_