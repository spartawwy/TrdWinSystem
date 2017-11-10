#ifndef LOG_MSG_MODULE_H_
#define LOG_MSG_MODULE_H_

class WinnerRelayApp;

class LogMsgModule
{
public:

	explicit LogMsgModule(WinnerRelayApp& app);

	void Initiate();

private:

	WinnerRelayApp&			app_;
};

#endif // LOG_MSG_MODULE_H_