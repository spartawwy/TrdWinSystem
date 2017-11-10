#include <iostream>

#include <TLib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>

#include "winner_relay_app.h"

class TheFrame: public TSystem::ExecutableFrame
{
private:

	virtual int main(int argc, char* argv[])
	{
		std::string usage("winner_relay.exe <sys_relay resource name>");
		if( argc < 2 )
		{
			std::cout << usage << std::endl;
			return 1;
		}

		TSystem::utility::ProjectTag(PROJECT_TAG_);

		WinnerRelayApp app(argv[1], "1.0");
		app.Initiate();
		PrintAppInfo(app);

		app.RollState();
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
