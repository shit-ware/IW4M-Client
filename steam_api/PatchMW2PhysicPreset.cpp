// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Anything and everything related to PhysicsPresets
//
// Initial author: TheApadayo
// Started: 2014-05-20
// ==========================================================
#include "StdInc.h"

void PatchMW2PhysicPreset()
{

}

typedef struct
{
	const char *name;
	int type;
	float mass;
	float bounce;
	float friction;
	float bulletForceScale;
	float explosiveForceScale;
	const char *sndAliasPrefix;
	float piecesSpreadFraction;
	float piecesUpwardVelocity;
	bool tempDefaultToCylinder;
} PhysPreset;

void LoadPhysicPreset(void* data)
{
	PhysPreset* phys = (PhysPreset*)data;

	if (GAME_FLAG(GAME_FLAG_DUMPDATA))
	{
		char * info = new char[4096];

		FILE* dump = fopen(va("data/physic/%s", phys->name), "w");
		fprintf(dump, "PHYSIC\\");
		fprintf(dump, "type\\%d\\", phys->type);
		fprintf(dump, "sndAliasPrefix\\%s\\", phys->sndAliasPrefix);
		fprintf(dump, "mass\\%g\\", phys->mass);
		fprintf(dump, "friction\\%g\\", phys->friction);
		fprintf(dump, "bounce\\%g\\", phys->bounce);
		fprintf(dump, "bulletForceScale\\%g\\", phys->bulletForceScale);
		fprintf(dump, "explosiveForceScale\\%g\\", phys->explosiveForceScale);
		fprintf(dump, "piecesSpreadFraction\\%g\\", phys->piecesSpreadFraction);
		fprintf(dump, "piecesUpwardVelocity\\%g\\", phys->piecesUpwardVelocity);
		fprintf(dump, "tempDefaultToCylinder\\%d", phys->tempDefaultToCylinder);
		fclose(dump);
	}

	char* infoString;
	int len = FS_ReadFile(va("physic/%s", phys->name), &infoString);
	if (len < 0)
		return;

	if (strncmp("PHYSIC\\", infoString, 7)) {
		Com_Printf(0, "Error loading PhysPreset %s\n", phys->name);
		return;
	}

	infoString += 7;

	phys->type = atoi(Info_ValueForKey(infoString, "type"));
	phys->sndAliasPrefix = strdup(Info_ValueForKey(infoString, "sndAliasPrefix"));
	phys->mass = atof(Info_ValueForKey(infoString, "mass"));
	phys->friction = atof(Info_ValueForKey(infoString, "friction"));
	phys->bounce = atof(Info_ValueForKey(infoString, "bounce"));
	phys->bulletForceScale = atof(Info_ValueForKey(infoString, "bulletForceScale"));
	phys->explosiveForceScale = atof(Info_ValueForKey(infoString, "explosiveForceScale"));
	phys->piecesSpreadFraction = atof(Info_ValueForKey(infoString, "piecesSpreadFraction"));
	phys->piecesUpwardVelocity = atof(Info_ValueForKey(infoString, "piecesUpwardVelocity"));
	phys->tempDefaultToCylinder = atof(Info_ValueForKey(infoString, "tempDefaultToCylinder"));
}

void AddPhysPresetToModel(void* data)
{
	XModel* mod = (XModel*)data;
	if (FS_ReadFile(va("physic/%s", mod->name), NULL) < 0) return;

	mod->physPreset = new PhysPreset;
	((PhysPreset*)mod->physPreset)->name = strdup(mod->name);
	LoadPhysicPreset(mod->physPreset);
}