// TradeServer.cpp : 定义控制台应用程序的入口点。
//

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>

#include "interm_common.h"

//#include "trade_server_app.h"


class TheFrame : public TSystem::ExecutableFrame
{

public:
	int main(int argc, char* argv[])
	{
		TSystem::utility::ProjectTag("WZF");

		/*TradeServerApp  app;

		PrintAppInfo(app);

		PrintLaunchDone();*/

		app.WaitShutdown();

		return 0;

	}
};


int main(int argc, char* argv[])
{
	TheFrame frame;

	return frame.LaunchServer(argc, argv);
}