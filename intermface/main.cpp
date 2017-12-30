 
#include <iostream>

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>
#include <TLib/tool/tsystem_cmdoption.h>

#include "interm_common.h"

#include "intermface.h"


class TheFrame : public TSystem::ExecutableFrame
{

public:
	int main(int argc, char* argv[])
	{
		TSystem::utility::ProjectTag("WZF");
		  
		std::string usage("Usage: intermface [work mode] <trade_server name> ");
		if( argc < 2 )
		{
			std::cout << usage << std::endl;
			return 1;
		}

		// add option
		TSystem::utility::CmdLineOption cmd_line_option;
		cmd_line_option.AddOption("monitor", "work mode: only output QuoteFill data");
		std::cout << cmd_line_option.PrintHelp() << std::endl;

		// parse argv
		TSystem::utility::ParsedOptions suffix = cmd_line_option.ParseArgv(argv+1, argc-1);
		if( suffix.remain().size() < 1 )
		{
			std::cout << usage << std::endl;
			return 1;
		}

		Intermface app(suffix.Exist("monitor"));
		app.Initiate();
		PrintAppInfo(app);

		app.Start(suffix.remain()[0]);
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