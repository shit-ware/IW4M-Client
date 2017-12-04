// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: ISteamFriends005 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-27
// ==========================================================

#include "StdInc.h"
#include "SteamFriends005.h"
#include <time.h>
#include <Winsock2.h>

static char g_userName[256];
static bool g_nameSet = false;

char* Auth_GetUsername();

const char *CSteamFriends005::GetPersonaName()
{
	if (GAME_FLAG(GAME_FLAG_DEDICATED))
	{
		return VERSIONSTRING "d";
	}

	return Auth_GetUsername();
}

void CSteamFriends005::SetPersonaName( const char *pchPersonaName )
{
}

EPersonaState CSteamFriends005::GetPersonaState()
{
	return k_EPersonaStateOnline;
}

int CSteamFriends005::GetFriendCount( EFriendFlags iFriendFlags )
{
	return NP_GetNumFriends();
}

CSteamID CSteamFriends005::GetFriendByIndex( int iFriend, int iFriendFlags )
{
	return CSteamID(NP_GetFriend(iFriend));
}

EFriendRelationship CSteamFriends005::GetFriendRelationship( CSteamID steamIDFriend )
{
	if (NP_GetFriendName(steamIDFriend.ConvertToUint64()) != NULL)
	{
		return k_EFriendRelationshipFriend;
	}
	return k_EFriendRelationshipNone;
}

EPersonaState CSteamFriends005::GetFriendPersonaState( CSteamID steamIDFriend )
{
	EPresenceState presence = NP_GetFriendPresence(steamIDFriend.ConvertToUint64());

	switch (presence)
	{
	case PresenceStateOnline:
		return k_EPersonaStateOnline;
	case PresenceStateAway:
		return k_EPersonaStateAway;
	case PresenceStateExtendedAway:
		return k_EPersonaStateSnooze;
	}

	return k_EPersonaStateOffline;
}

const char *CSteamFriends005::GetFriendPersonaName( CSteamID steamIDFriend )
{
	return NP_GetFriendName(steamIDFriend.ConvertToUint64());
}

int CSteamFriends005::GetFriendAvatar( CSteamID steamIDFriend, int eAvatarSize )
{
	return 0;
}

int GetLocalPort(int sockfd, unsigned short& port)
{
	struct sockaddr_in sin;
	int sinlen = sizeof(sin);

	memset(&sin, 0, sizeof(sin));

	if (getsockname(sockfd, (struct sockaddr*) &sin, &sinlen) == 0 && sin.sin_family == AF_INET)
	{
		port = ntohs(sin.sin_port);
		return 0;
	}

	return 4;
}

bool CSteamFriends005::GetFriendGamePlayed( CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo )
{
	const char* friendGame = NP_GetFriendRichPresence(steamIDFriend.ConvertToUint64(), "game");

	if (friendGame && !stricmp(friendGame, "iw4m"))
	{
		pFriendGameInfo->m_gameID = CGameID(10190);
		pFriendGameInfo->m_steamIDLobby = CSteamID(steamIDFriend.GetAccountID(), 0x40000, k_EUniversePublic, k_EAccountTypeChat);
		pFriendGameInfo->m_unGameIP = inet_addr(NP_GetFriendRichPresence(steamIDFriend.ConvertToUint64(), "current_server"));
		GetLocalPort(pFriendGameInfo->m_unGameIP, pFriendGameInfo->m_usGamePort);
		pFriendGameInfo->m_usQueryPort = pFriendGameInfo->m_usGamePort;
		Com_Printf(0, "m_unGameIP: %d, m_usGamePort: %d", pFriendGameInfo->m_unGameIP, pFriendGameInfo->m_usGamePort);
	}
	return true;
}

const char *CSteamFriends005::GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName )
{
	

	return "";
}

bool CSteamFriends005::HasFriend( CSteamID steamIDFriend, EFriendFlags iFriendFlags )
{
	

	return false;
}

int CSteamFriends005::GetClanCount()
{
	

	return 0;
}

CSteamID CSteamFriends005::GetClanByIndex( int iClan )
{
	

	return CSteamID();
}

const char *CSteamFriends005::GetClanName( CSteamID steamIDClan )
{
	

	return "c14n";
}

int CSteamFriends005::GetFriendCountFromSource( CSteamID steamIDSource )
{
	

	return 0;
}

CSteamID CSteamFriends005::GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend )
{
	

	return CSteamID();
}

bool CSteamFriends005::IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource )
{
	

	return false;
}

void CSteamFriends005::SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking )
{
	
}

void CSteamFriends005::ActivateGameOverlay( const char *pchDialog )
{

}

void CSteamFriends005::ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID )
{
	
}

void CSteamFriends005::ActivateGameOverlayToWebPage( const char *pchURL )
{
	
}

void CSteamFriends005::ActivateGameOverlayToStore( AppId_t nAppID )
{
	
}

void CSteamFriends005::SetPlayedWith( CSteamID steamIDUserPlayedWith )
{

}