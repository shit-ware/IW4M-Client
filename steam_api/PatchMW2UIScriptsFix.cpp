// ==========================================================
// aiw3 project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Fix damaged UI Scripts
//
// Initial author: momo5502
//
// Started: 2013-12-01
// ==========================================================

#include "StdInc.h"
#include "PatchMW2UIScripts.h"

using namespace std;

void Quit_UIScript_CleanUp(char* args)
{
	// I guess that's what it's supposed to do
	Cmd_ExecuteSingleCommand(0, 0, "uploadStats");
	Cmd_ExecuteSingleCommand(0, 0, "quit");
}

struct mapArena_t
{
	char uiName[32];
	char mapName[16];
	char pad[2768];
};

static int* numArenas = (int*)0x62E6930;
static mapArena_t* arenas = (mapArena_t*)0x62E6934;

vector<string> explode(const string& str, const string& delimiters = " ")
{
	vector<string> tokens;

	auto subStrBeginPos = str.find_first_not_of(delimiters, 0);
	auto subStrEndPos = str.find_first_of(delimiters, subStrBeginPos);

	while (string::npos != subStrBeginPos || string::npos != subStrEndPos)
	{
		tokens.push_back(str.substr(subStrBeginPos, subStrEndPos - subStrBeginPos));

		subStrBeginPos = str.find_first_not_of(delimiters, subStrEndPos);
		subStrEndPos = str.find_first_of(delimiters, subStrBeginPos);
	}

	return tokens;
}

string getBasemapValueForKey(const char* buffer, char* key)
{
	auto bufSplit = explode(buffer, "\"");

	for (int i = 0; i<bufSplit.size(); i += 2)
	{
		if (bufSplit[i].find(key) != -1)
		{
			return bufSplit[i + 1];
		}
	}

	return "";
}

void LoadArenas_UIScript_CleanUp(char* FuckTheseArgs)
{
	char* buffer;
	int length = FS_ReadFile("mp/basemaps.arena", &buffer);

	if (length == 0)
	{
		*numArenas = 0;
		return;
	}

	int count = 0;
	auto partialMapSplit = explode(buffer, "}");

	for (int i = 0; i < partialMapSplit.size(); i++)
	{
		strncpy(arenas[i].mapName, getBasemapValueForKey(partialMapSplit[i].c_str(), "map").c_str(), sizeof(arenas[i].mapName));
		strncpy(arenas[i].uiName, getBasemapValueForKey(partialMapSplit[i].c_str(), "longname").c_str(), sizeof(arenas[i].uiName));
		count++;
	}

	*numArenas = count;
}

void PatchMW2_UIScriptFix()
{
	// At least better than menufile modifications
	//AddUIScript("quit", Quit_UIScript_CleanUp);
	AddUIScript("LoadArenas", LoadArenas_UIScript_CleanUp); // Use this till we found the actual problem
}