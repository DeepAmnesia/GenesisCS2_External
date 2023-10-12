#pragma once
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment( lib, "ws2_32.lib" )

enum MessageType : int
{
	LoaderAuth = 1,
	CheatAuth = 2,
	GetConfigList = 3,
	GetConfig = 4
};

class C_RequestManager
{
public:
	virtual void CreateConnection( SOCKET& m_Socket, sockaddr_in& m_SocketAddress);
	virtual int SendBuffer( SOCKET m_Socket, void* Buffer, int nSendLength );
	virtual int RecvBuffer( SOCKET m_Socket, void* Buffer, int nMaxSize );

};

inline C_RequestManager* g_RequestManager = new C_RequestManager( );