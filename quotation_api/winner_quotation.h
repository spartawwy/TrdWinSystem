#ifndef WINNER_QUOTATION_DSFDSh_H_
#define WINNER_QUOTATION_DSFDSh_H_

//#include "winner_quotation_api.h"
#include <string>

class WinnerQuotation
{
public:
    WinnerQuotation();
    ~WinnerQuotation();

    void Initiate();

    void MainLoop();

    void Shutdown();

    int Register(void *quote_call_back, unsigned int each_call_back_delay, char *code, int start_date, int start_hhmm);

private:
    void Handle();

private:
    bool exist_flat_;

    void *quote_call_back_;
    unsigned int each_call_back_delay_;

    std::string code_;
    int start_date_;
    int start_hhmm_;
};

void Delay(unsigned short mseconds);

#endif