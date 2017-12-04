// ==========================================================
// REKT.IW4 project
//
// Component: clientdll
// Sub-component: steam_api
// Purpose: Port In-Game CAC by zxz0O0
// Original: https://github.com/dosomder/iw4m-ingame-cac
//
// Initial author: dosomder
// Started: 2014-08-05
// ==========================================================

#include "StdInc.h"

void PatchMW2_IGCAC() {
	// Allow in-game stat modifying
	*(DWORD*)0x4351C9 = 0xC39001B0;
}