// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Colors Prestige 11 ( Staff )
//
// Initial author: RaidMax
// Started: 2014-1-1
// ==========================================================

#include "StdInc.h"
#include "StdAfx.h"

#define NAKED __declspec(naked) 

class C_Client
{
public:
	int IsValid; 			// 0x0000 
	int IsValid2; 			// 0x0004 
	int ClientNumber; 		// 0x0008 
	char Name[16]; 			// 0x000C 
	int Team; 				// 0x001C 
	int Team2; 				// 0x0020 
	int Rank; 				// 0x0024 (rank - 1)
	int Prestige; 			// 0x0028
	int Perks; 				// 0x002C 
	int Kills; 				// 0x0030
	int Score; 				// 0x0034 
	char _0x0038[968];
	int ViewAngles; 		// 0x0400 
	char _0x040C[136];
	int IsShooting; 		// 0x0494 
	char _0x0498[4];
	int IsZoomed; 			// 0x049C 
	char _0x04A0[68];
	int weaponID; 			// 0x04E4 
	char _0x04E8[24];
	int weaponID2; 			// 0x0500 
	char _0x0504[40];
};							// 0x052C (total size)						// 0x052C (total size)

typedef struct
{
	char pad1[8];
	__int64 xuid;
	char pad2[40];
} C_sessionPlayer_t;

typedef struct
{
	char stuff[560];
	C_sessionPlayer_t players[17];
} C_session_t;

C_session_t* ingameSession = (C_session_t*)0x66B7008;

bool NP_STAFF_UPDATED = false;

void CheckPrestige(C_Client* client)
{
	__int64 clientGUID = ingameSession->players[client->ClientNumber].xuid;

	if (client->Prestige == 11)
	{
		//char* material = "gradient";
		//*(DWORD*)0x590E60 = (DWORD)material;
		*(DWORD*)0x590E60 = 0x0072D0A068; // "black" material (push 0x72D0A0)
	}
	else {
		*(DWORD*)0x590E60 = 0x006F5EE868; // Default "white" material (push 0x6F5EE8)
	}

}

StompHook StaffColorHook;
DWORD CG_DrawClientScoreLoc = 0x590E10; // call 0x4F8940
DWORD CG_DrawClientScoreRet = 0x590E15; // mov esi, eax
DWORD FunctionLoc = 0x4F8940;


float Red_color[4] = { 1, 0, 0, 1 };
float Green_color[4] = { 0, 1, 0, 1 };
float DarkBlue_color[4] = { 0, 0, 1, 1 };
float Yellow_color[4] = { 1, 1, 0, 1 };

DWORD JumpBack_GetColor = 0;
DWORD dwCall_GetColor = 0x423F70;
DWORD OffsetPtr = 0;

int ClientID;
bool returnval;

bool SetColorPlayer(int id)
{
	/*
	C_Client* client = (C_Client*)(0x8EB248 + (0x52C * id));
	float* Ptr = (float*)OffsetPtr;
	if (client->Prestige == 11)//DEV
	{ //COLOR_CPY(Ptr,Red_color);
	Ptr[0] = Red_color[0];
	Ptr[1] = Red_color[1];
	Ptr[2] = Red_color[2];
	Ptr[3] = Red_color[3];
	return true;
	}
	if (client->Prestige == 76)//DEV
	{ //COLOR_CPY(Ptr,Red_color);
	Ptr[0] = Red_color[0];
	Ptr[1] = Red_color[1];
	Ptr[2] = Red_color[2];
	Ptr[3] = Red_color[3];
	return true;
	}
	else if (client->Prestige == 79) //STAFF
	{ //COLOR_CPY(Ptr,Green_color);
	Ptr[0] = Green_color[0];
	Ptr[1] = Green_color[1];
	Ptr[2] = Green_color[2];
	Ptr[3] = Green_color[3];
	return true;
	}
	else if (client->Prestige == 54) //DONATOR
	{
	Ptr[0] = DarkBlue_color[0];
	Ptr[1] = DarkBlue_color[1];
	Ptr[2] = DarkBlue_color[2];
	Ptr[3] = DarkBlue_color[3];
	return true;
	}
	else if (client->Prestige == 95) //MODERATOR
	{
	Ptr[0] = Yellow_color[0];
	Ptr[1] = Yellow_color[1];
	Ptr[2] = Yellow_color[2];
	Ptr[3] = Yellow_color[3];
	return true;
	}

	else
	{
	return false;
	}
	*/
	float* Ptr = (float*)OffsetPtr;
	Ptr[0] = Red_color[0];
	Ptr[1] = Red_color[1];
	Ptr[2] = Red_color[2];
	Ptr[3] = Red_color[3];
	return true;
}

extern DWORD MakeJMP(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen);

void NAKED hkGetScoreboardcolor()
{
	__asm
	{
		mov ClientID, edi;
		mov OffsetPtr, ecx;
	}
	returnval = SetColorPlayer(ClientID);

	if (!returnval)
		__asm call[dwCall_GetColor];

	__asm jmp[JumpBack_GetColor];

}

void __declspec(naked) CG_DrawClientScoreStub()
{
	__asm
	{
		push eax					// -Lets save eax and set the current iteration client as the parameter for `CheckPrestige`
			call CheckPrestige			// -Calling CheckPrestige(eax)
			pop eax						// -Restore eax just incase we need this register later on (hint, we do)

			call FunctionLoc			// -Call the `function` we over-wrote at 0x590E10
			jmp CG_DrawClientScoreRet	// -Jump back to original hook location + 0x5
	}
}

void PatchMW2_StaffColour()
{
	StaffColorHook.initialize(CG_DrawClientScoreLoc, CG_DrawClientScoreStub);
	StaffColorHook.installHook();
	//JumpBack_GetColor = MakeJMP((PBYTE)0x00597623, (DWORD)hkGetScoreboardcolor, 5); doesn't work for some reason

	//*(int*)0x640D1D0 = 9;	// Font = 9
	//*(int*)0x640D1E0 = 9;	// Font = 9
}