#include <algorithm>

#include "winner_message_system.h"
#include "winner_message_classid.h"

void WINNERSystem::LoadClassID(TSystem::MessageSystem& system)
{
	int class_id = 0;
	std::for_each( ClassIDArray, ClassIDArray+sizeof(ClassIDArray)/sizeof(const char*)
		, [&system, &class_id](const char* name){system.RegisterClassID(name, ++class_id);});
}