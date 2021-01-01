#include "winner_quotation.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <thread>
#include <chrono>

#include <windows.h>

static WinnerQuotation* GetInstance(bool is_del = false);

WinnerQuotation::WinnerQuotation()
    : exist_flat_(false)
    , quote_call_back_(nullptr)
    , each_call_back_delay_(0)
    , start_date_(0)
    , start_hhmm_(0)
{

}

WinnerQuotation::~WinnerQuotation()
{
    exist_flat_ = true;
}

void WinnerQuotation::Initiate()
{

}

int WinnerQuotation::Register(void *quote_call_back, unsigned int each_call_back_delay, char *code, int start_date, int start_hhmm)
{
    quote_call_back_ = quote_call_back;
    each_call_back_delay_ = each_call_back_delay;
    code_ = code;
    start_date_ = start_date;
    start_hhmm_ = start_hhmm;
    return 1;
}

void WinnerQuotation::MainLoop()
{
    while(!exist_flat_)
    {
        Sleep(500);
        if( quote_call_back_ && start_date_ > 1900 )
        {
            Handle();
        }
    }
}

void WinnerQuotation::Shutdown()
{
    exist_flat_ = true;
}

void WinnerQuotation::Handle()
{
#if 0
    std::string date_format_str = utility::FormatStr("%d", date);
    std::string year_mon = utility::FormatStr("%d%02d", (date/10000), date%10000/100);
    std::string year_mon_sub = year_mon + (IsShStock(req->code()) ? "SH" : "SZ");
    std::string full_path = this->stk_data_dir_ + year_mon + "/" + year_mon_sub + "/" + date_format_str + "/" + req->code() + "_" + date_format_str + ".csv";
#endif

    std::string full_path = "F:/StockHisdata/SCL8/201901/20190102/sc1903_20190102.csv";

    //std::string id;
    int date;
    int hour;
    int minute;
    int second;
    double price;
    int change_price;
    int cur_total_vol;
    //std::string amount; 

    std::string partten_string =
    "^(\\d{8}),(\\S+),(\\S+),(\\s+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+),(\\d{2}):(\\d{2}):(\\d{2}),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(.*)$";

    std::regex regex_obj(partten_string); 
     
    char buf[1024] = {0};
    std::fstream in(full_path);
    if( !in.is_open() )
    {
        printf("open file fail");
        getchar();
        return;
    }
    int line = 0;
    while( !in.eof() )
    { 
        if( line++ % 100 == 0 )
            Delay(1);

        in.getline(buf, sizeof(buf));
        /*std::cout << buf << std::endl;
        continue;*/

        if( line == 1 )
            continue;

        //Delay(each_call_back_delay_);
        
        int len = strlen(buf);
        if( len < 1 )
            continue;
        char *p0 = buf;
        char *p1 = &buf[len-1];
        std::string src(p0, p1);

        std::smatch result; 
        if( std::regex_match(src.cbegin(), src.cend(), result, regex_obj) )
        {  //std::cout << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << std::endl;
            date = std::stoi(result[1]);
            hour = std::stoi(result[21]);   //result[21];
            minute = std::stoi(result[22]); //result[22]; 
            second = std::stoi(result[23]); //result[23];
            price = std::stod(result[5]);   //result[5];
            //change_price = "0.0";
            cur_total_vol = std::stoi(result[12]); //result[12];
            std::cout << date << " " << hour << ":" << minute << ":" << second << " " << price << " " << cur_total_vol << std::endl;
            try
            {
#if 0
                QuotationMessage::QuotationFillMessage * p_fill_msg = quotation_msg.add_quote_fill_msgs();
                auto date_comp = TSystem::FromLongdate(date);
                FillTime( TimePoint(TSystem::MakeTimePoint(std::get<0>(date_comp), std::get<1>(date_comp), std::get<2>(date_comp)
                    , std::stoi(hour), std::stoi(minute), std::stoi(second)))
                    , *p_fill_msg->mutable_time() );
                TSystem::FillRational(price, *p_fill_msg->mutable_price()); 
                TSystem::FillRational(change_price, *p_fill_msg->mutable_price_change());

                if( std::stod(change_price) < 0.0 )
                    p_fill_msg->set_is_change_positive(false);
                p_fill_msg->set_vol(std::stoi(vol));  
                // push to code_fenbi_container
                int hhmmss = std::stoi(hour)*10000 + std::stoi(minute)*100 + std::stoi(second);
                day_iter->second->push_back(std::move(T_Fenbi(hhmmss, std::stof(price))));
#endif

            }catch(std::exception &e)
            {
                printf("exception:%s", e.what());
                continue;
            }catch(...)
            {
                continue;
            }
        }
        //--------------------
        if( *p1 == '\0' ) break; 
        if( int(*p1) == 0x0A ) // filter 0x0A
            ++p1;
    } // while
}



void Delay(unsigned short mseconds)
{
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}