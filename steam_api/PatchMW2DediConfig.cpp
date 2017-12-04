// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Persistent dedicated server configuration.
//
// Initial author: NTAuthority
// Started: 2013-03-14
// ==========================================================

#include "StdInc.h"

static char configBaseName[256];

typedef void (__cdecl * Dvar_ForEach_t)(void (*cb)(dvar_t*, void*), void* userData);
Dvar_ForEach_t Dvar_ForEach = (Dvar_ForEach_t)0x4360C0;

typedef const char* (__cdecl * Dvar_DisplayableLatchedValue_t)(dvar_t* dvar);
Dvar_DisplayableLatchedValue_t Dvar_DisplayableLatchedValue = (Dvar_DisplayableLatchedValue_t)0x4AC100;

void Com_SaveDediConfig_Each(dvar_t* dvar, void* userData)
{
	FILE* f = *(FILE**)userData;

	if (dvar->flags & DVAR_FLAG_DEDISAVED)
	{
		fprintf(f, "seta %s \"%s\"\n", dvar->name, Dvar_DisplayableLatchedValue(dvar));
	}
}

void Com_SaveDediConfig()
{
	FILE* f = fopen(va("zone/%s", configBaseName), "w");

	if (f)
	{
		fprintf(f, "// generated by IW4M, modify with extreme care\n");

		Dvar_ForEach(Com_SaveDediConfig_Each, &f);

		fclose(f);
	}
}

void Com_WriteDediConfig()
{
	if (*(DWORD*)0x1AD8F68) // fully initialized?
	{
		if (!*(BYTE*)0x1AD8F94) // invalid dvar write
		{
			char* dvarLocal = (char*)((*(DWORD*)__readfsdword(44)) + (4 * *(DWORD*)0x66D94A8));
			DWORD* modifiedFlags = (DWORD*)(dvarLocal + 24);

			if (*modifiedFlags & DVAR_FLAG_DEDISAVED)
			{
				Com_SaveDediConfig();
				*modifiedFlags &= ~DVAR_FLAG_DEDISAVED;
			}
		}
	}
}

typedef void (__cdecl * Cbuf_ExecuteNow_t)(int a1, int a2, const char* string);
Cbuf_ExecuteNow_t Cbuf_ExecuteNow = (Cbuf_ExecuteNow_t)0x47AFA0;

void InitializeDediConfig()
{
	// list the command line arguments
	const char** args = (const char**)0x1AD7948;

	for (int i = 0; i < *(int*)0x1AD7EBC; i++)
	{
		if (!_strnicmp(args[i], "exec ", 5))
		{
			// this is an exec command
			const char* configName = &args[i][5];
			strcpy(configBaseName, configName);

			char* endChar = strrchr(configBaseName, '.');
			
			if (endChar)
			{
				endChar[0] = '\0';
			}
			else
			{
				endChar = strrchr(configBaseName, ' ');

				if (endChar)
				{
					endChar[0] = '\0';
				}
			}

			strcat(configBaseName, ".iw4cfg");
		}
	}

	// if we didn't set the basename, set it using the net_port value
	if (!configBaseName[0])
	{
		dvar_t* net_port = Dvar_FindVar("net_port");

		if (net_port)
		{
			_snprintf(configBaseName, sizeof(configBaseName), "auto_%s.iw4cfg", net_port->current.string); // it's a string for now as the dvar hasn't been registered yet
		}
		else
		{
			strcpy(configBaseName, "auto_28960.iw4cfg");
		}
	}

	Dvar_RegisterInt("sv_ownerUserID", 0, 0, INT_MAX, DVAR_FLAG_DEDISAVED, "Forum user ID of the server's main administrator.");

	// load the config file, then
	FILE* f = fopen(va("zone/%s", configBaseName), "r");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		int length = ftell(f);
		
		fseek(f, 0, SEEK_SET);

		char* buffer = new char[length + 1];
		fread(buffer, 1, length, f);
		buffer[length] = '\0';

		Cbuf_ExecuteNow(0, 0, buffer);

		delete[] buffer;
	}

	char strippedBaseName[256];
	strcpy_s(strippedBaseName, sizeof(strippedBaseName), configBaseName);
	
	char* stripDot = strrchr(strippedBaseName, '.');

	if (stripDot)
	{
		stripDot[0] = '\0';
	}

	Dvar_RegisterString("sv_baseConfigName", strippedBaseName, DVAR_FLAG_READONLY, "Server configuration file name.");

	// we also patch from here, just because we can
	//call(0x60B29B, Com_WriteDediConfig, PATCH_JUMP);
	call(0x60B240, Com_WriteDediConfig, PATCH_JUMP);
}