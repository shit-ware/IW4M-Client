#include "StdInc.h"
#include "PatchMW2UIScripts.h"

dvar_t* auto_demo;

void autoDemoRecord(char* args)
{
	if(auto_demo->current.boolean)
	{
			Cmd_ExecuteSingleCommand(0,0,"record");
	}
}

void PatchMW2_AutoDemo()
{
	AddUIScript("autoRecord",autoDemoRecord);

	auto_demo = (dvar_t*)Dvar_RegisterBool("auto_demo", 0, DVAR_FLAG_SAVED, "Enable automatic demo record");
}