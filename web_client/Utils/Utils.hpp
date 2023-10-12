#pragma once
#include <WS2tcpip.h>
#include <TlHelp32.h>
#include <strsafe.h>

#include "../Helpers/Json.hpp"
#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

class C_Utils
{
public:
	virtual std::string GetHWID( );
	virtual void SendAuthMessage( SOCKET m_Socket, const char* szToken, char GameId[32] );
	virtual void AskConfigList( SOCKET m_Socket );
	virtual void AskConfig( SOCKET m_Socket, int id );
	virtual void SelfDelete();
};

inline C_Utils* g_Utils = new C_Utils( );