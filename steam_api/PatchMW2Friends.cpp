// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Friend list support.
//
// Initial author: NTAuthority
// Started: 2012-06-11
// ==========================================================
//Todo check if the game is iw4m
#include "StdInc.h"
#include "PatchMW2UIScripts.h"
#include "ServerStorage.h"

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_FRIENDS 1024

using namespace std;

char* Auth_GetSessionID();

static struct
{
	std::unordered_map<int, NPFriend> friends;//Holds all the friends data
	int selectedIndex;//Index of friendProfiles that is selected selected
	NPFriend selectedFriend;
} g_friends;

std::string htmlEntitiesDecode(std::string str) {

	std::string subs[] = {
		"& #34;", "&quot;",
		"& #39;", "&apos;",
		"& #38;", "&amp;",
		"& #60;", "&lt;",
		"& #62;", "&gt;",
		"&34;", "&39;",
		"&38;", "&60;",
		"&62;"
	};

	std::string reps[] = {
		"\"", "\"",
		"'", "'",
		"&", "&",
		"<", "<",
		">", ">",
		"\"", "'",
		"&", "<",
		">"
	};

	size_t found;
	for (int j = 0; j < 15; j++) {
		do {
			found = str.find(subs[j]);
			if (found != std::string::npos)
				str.replace(found, subs[j].length(), reps[j]);
		} while (found != std::string::npos);
	}
	return str;
}

void Friends_StatsCB(NPAsync<NPFriendResult>* async)
{
	NPFriendResult* result = async->GetResult();
	std::string empty = "";

	for (int i = 0; i < result->numResults; i++)
	{
		//Add to our local friend list
		g_friends.friends[i] = result->friends[i];

		std::string sourceIPPORT(g_friends.friends[i].current_server);
		std::string host, port;

		if (g_friends.friends[i].hostname == "" || sourceIPPORT.length() == 0 || g_friends.friends[i].hostname == "none" || sourceIPPORT.empty()) {
			//Com_Printf(0, "user: %s: host: %s current: %s RESET.\n", g_friends.friends[i].username, g_friends.friends[i].hostname, g_friends.friends[i].current_server);
			strncpy(g_friends.friends[i].current_server, empty.c_str(), sizeof(empty));
			strncpy(g_friends.friends[i].hostname, empty.c_str(), sizeof(empty));
			continue;
		}
		strncpy(g_friends.friends[i].hostname, htmlEntitiesDecode(g_friends.friends[i].hostname).c_str(), sizeof(g_friends.friends[i].hostname));
	}
	Com_Printf(0, "Loaded %i friends.\n", result->numResults);
}

void Friends_LoadFriends(char* args)
{
	NPAsync<NPFriendResult>* async = NP_GetFriends();
	async->SetCallback(Friends_StatsCB, NULL);
}

StompHook cgParseServerInfoHook;
DWORD cgParseServerInfoHookLoc = 0x4CD023;
extern int* demoPlaying;

void CG_ParseServerInfoHookFunc()
{
	NP_SetRichPresence("current_server", CL_GetServerIPAddress());
	NP_StoreRichPresence();

	// not related to friends but as we're hooking here anyway
	netadr_t* adr = (netadr_t*)0xA1E888;
	ServerStorage_AddEntry(&historyList, *adr, (const char*)0x7ED3F8);
}

CallHook clDisconnectHook;
DWORD clDisconnectHookLoc = 0x403582;

void CL_DisconnectHookFunc()
{
	Cmd_ExecuteSingleCommand(0, 0, "stoprecord");

	NP_SetRichPresence("hostname", "");
	NP_SetRichPresence("current_server", "");
	//NP_SendRandomString("dis");
	NP_StoreRichPresence();

	//He just left, good chance he might wanna hook up with a friend. Lets get some the latest friend data
	Friends_LoadFriends("");
}

void __declspec(naked) CL_DisconnectHookStub()
{
	__asm
	{
		call CL_DisconnectHookFunc
			jmp clDisconnectHook.pOriginal
	}
}

CallHook clShutdownOnceForAllClientsHook;
DWORD clShutdownOnceForAllClientsHookLoc = 0x4ECB83;

void CL_ShutdownOnceForAllClientsHookFunc()
{
	NP_SetRichPresence("hostname", "");
	NP_SetRichPresence("current_server", "");
	NP_SetRichPresence("isonline", "");
	NP_StoreRichPresence();
}


void __declspec(naked) CL_ShutdownOnceForAllClientsHookStub()
{
	__asm
	{
		call CL_ShutdownOnceForAllClientsHookFunc
			jmp clShutdownOnceForAllClientsHook.pOriginal
	}
}

CallHook clShutdownHook;
DWORD clShutdownHookLoc = 0x42BF90;

void __declspec(naked) CL_ShutdownHookStub()
{
	__asm
	{
		call CL_ShutdownOnceForAllClientsHookFunc
			jmp clShutdownHook.pOriginal
	}
}

//TODO: Make the first row a clan/flag icon
int timer = 0;
const char* FriendList_GetItemText(int index, int column)
{
	NPFriend aFriend = g_friends.friends[index];
	if (aFriend.id == 0)
		return "ERROR";

	if (timer >= 30000) {
		Friends_LoadFriends("");
		timer = 0;
	}
	timer++;

	switch (column)
	{
	case 0:
	{
			  return aFriend.username;
	}
	case 1:
	{
			  return "";
	}
	case 2:
	{
			  return "";
	}
	case 3:
	{
			  std::string hostname(aFriend.hostname);
			  //Com_Printf(0, "%s: %s.\n", aFriend.username, aFriend.hostname);
			  if (aFriend.isonline == 0)
				  return "^1Offline";
			  else if (hostname.length() > 0)
				  return aFriend.hostname;
			  else
				  return "^2Online";
	}
	}
	return "";
}

void FriendList_Select(int index)
{
	g_friends.selectedFriend = g_friends.friends[index];
}

void UI_friendAction(char* args)
{
	if (g_friends.selectedFriend.isonline == 1)
	{
		Cmd_ExecuteSingleCommand(0, 0, va("connect %s", g_friends.selectedFriend.current_server));
	}
	else
	{
		if (timer > 1500) {
			timer = 0;
			Friends_LoadFriends("");
		}
		Cbuf_AddText(0, "snd_playLocal exit_prestige\n");
	}
}

StompHook isClientInPartyHook;
DWORD isClientInPartyHookLoc = 0x493130;


int FriendList_GetItemCount()
{
	return g_friends.friends.size() - 1;
}

void PatchMW2_Friends()
{
	UIFeeder_t feeder;
	feeder.feeder = 15.0f;
	feeder.GetItemCount = FriendList_GetItemCount;
	feeder.GetItemText = FriendList_GetItemText;
	feeder.Select = FriendList_Select;

	UIFeeders.push_back(feeder);

	AddUIScript("friendAction", UI_friendAction);
	AddUIScript("loadFriends", Friends_LoadFriends);
	//AddUIScript("addFriend", Friends_inviteFriend);

	cgParseServerInfoHook.initialize(cgParseServerInfoHookLoc, CG_ParseServerInfoHookFunc);
	cgParseServerInfoHook.installHook();

	clDisconnectHook.initialize(clDisconnectHookLoc, CL_DisconnectHookStub);
	clDisconnectHook.installHook();

	clShutdownHook.initialize(clShutdownHookLoc, CL_ShutdownHookStub);
	clShutdownHook.installHook();

	clShutdownOnceForAllClientsHook.initialize(clShutdownOnceForAllClientsHookLoc, CL_ShutdownOnceForAllClientsHookStub);
	clShutdownOnceForAllClientsHook.installHook();
}