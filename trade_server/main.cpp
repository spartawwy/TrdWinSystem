#include <iostream>

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>

#include "sys_common.h"

#include "trade_server_app.h"

 
class TheFrame : public TSystem::ExecutableFrame
{
 
public:
    int main(int argc, char* argv[])
    {
        TSystem::utility::ProjectTag("WZF");

		std::string usage("trade_server.exe <trade_server resource name>");
		if( argc < 2 )
		{
			std::cout << usage << std::endl;
			return 1;
		}

        TradeServerApp  app(argv[1], "0.1");
		app.Initiate();

        PrintAppInfo(app);

        PrintLaunchDone();
         
        app.WaitShutdown();

        return 0;

    }
};


int main(int argc, char* argv[])
{
    TheFrame frame;

    return frame.LaunchServer(argc, argv);
}