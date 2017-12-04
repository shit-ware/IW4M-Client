// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Code to replace localized strings in the game.
// Edit by: Fl0w add the ability of loading custom str files.
// Initial author: NTAuthority
// Started: 2013-01-17
// ==========================================================

#include "StdInc.h"
#include "PatchMW2LocalizedStrings.h"
#include <fstream>

struct localizedEntry_s
{
	const char* value;
	const char* name;
};

StompHook loadLocalizeHook;
DWORD loadLocalizeHookLoc = 0x629B90;

void defineLocalizedStrings()
{
	_localizedStrings["EXE_AWAITINGGAMESTATE"] = "Awaiting gamestate";
	_localizedStrings["MPUI_NUKED"] = "Nuketown";
	_localizedStrings["MPUI_CRASH_TROP"] = "CRASH TROPICAL";
	_localizedStrings["MPUI_KILLHOUSE"] = "Killhouse";
	_localizedStrings["MPUI_FAV_TROPICAL"] = "FAVELA TROPICAL";
	_localizedStrings["MPUI_ESTATE_TROPICAL"] = "ESTATE TROPICAL";
	_localizedStrings["WEAPON_PEACEKEEPER"] = "Peacekeeper SMG";
	_localizedStrings["MENU_RECONNECTING_TO_PARTY"] = "CONNECTING TO SERVER";
	_localizedStrings["MPUI_BOG_SH"] = "Bog";
	_localizedStrings["MPUI_STORMSPRING"] = "STORM SPRING";
	_localizedStrings["EXE_HISTORY"] = "History";
	_localizedStrings["MPUI_AIRPORT"] = "Airport";
	_localizedStrings["MPUI_FAVELA2"] = "Favela SP";
	_localizedStrings["MPUI_ESTATE_MAK"] = "Estate SP";
	_localizedStrings["MPUI_SNIPE"] = "iSnipe";
	_localizedStrings["WEAPON_DRAGUNOV"] = "Dragunov";
	_localizedStrings["WEAPON_WINCHESTER"] = "Winchester";
	_localizedStrings["WEAPON_AK74"] = "COD4 AK47";
	_localizedStrings["WEAPON_W1200"] = "Winchester1200";
	_localizedStrings["WEAPON_COLT45"] = "M1911.45";
}
FILE* str;
std::vector<std::string> customLocalizedStrings;

std::string replaceString(std::string &s, std::string toReplace, std::string replaceWith)
{
	size_t pos = 0;
	while ((pos = s.find(toReplace, pos)) != std::string::npos) {
		s.replace(pos, toReplace.length(), replaceWith);
		pos += replaceWith.length();
	}
	return s;
}

bool isOdd(int i)
{

	if (i % 2 == 0)
		return true;
	else
		return false;
}

void loadCustomStringFiles()
{
	int count;
	char** list = 0;
	list = FS_ListFiles("localized_strings/", "str", 0, &count);

	for (int i = 0; i < count; i++)
	{
		Com_Printf(0, "Parsing localized string file: %s\n", list[i]);
		std::ifstream file(va(".\\m2demo\\localized_strings\\%s", list[i]));
		std::string str;
		if (!file) {
			Com_Printf(0, "Could not parse localized string file : %s\n", list[i]);
			return;
		}
		while (std::getline(file, str)) {
			if (!str.empty())
				customLocalizedStrings.push_back(str);
		}

		for (int i2 = 0; i2 < customLocalizedStrings.size(); i2++)
		{
			if (!isOdd(i2))
				continue;

			if (customLocalizedStrings[i2 + 1].find("\"") != std::string::npos)
				_localizedStrings[customLocalizedStrings[i2]] = replaceString(customLocalizedStrings[i2 + 1], "\"", "\0");
			else
				_localizedStrings[customLocalizedStrings[i2]] = customLocalizedStrings[i2 + 1];

		}

		Com_Printf(0, "Successfully parsed localized string file: %s\n", list[i]);
	}
}

void loadLanguagesFilesHookStub()
{
	//clear localized strings else game crash
	_localizedStrings.clear();

	defineLocalizedStrings();

	loadCustomStringFiles();
	__asm
	{
		leave
			mov eax, 0x41D885
			jmp eax
	}
}

const char* SEH_GetLocalizedString(const char* key)
{
	google::dense_hash_map<std::string, std::string>::const_iterator iter = _localizedStrings.find(key);

	if (iter != _localizedStrings.end())
	{
		return (*iter).second.c_str();
	}

	localizedEntry_s* entry = (localizedEntry_s*)DB_FindXAssetHeader(ASSET_TYPE_LOCALIZE, key);

	if (entry)
	{
		return entry->value;
	}

	return NULL;
}

void PatchMW2_LocalizedStrings()
{
	_localizedStrings.set_empty_key("");

	defineLocalizedStrings();

	loadLocalizeHook.initialize(loadLocalizeHookLoc, SEH_GetLocalizedString);
	loadLocalizeHook.installHook();

	call(0x41D860, loadLanguagesFilesHookStub, PATCH_JUMP);
}