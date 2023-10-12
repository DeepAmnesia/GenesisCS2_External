#pragma once
#include <iostream>

#include "../Utils/Utils.hpp"
#include "../Helpers/XorStr.hpp"
#include "../Helpers/lazy_importer.hpp"

class C_Context
{
public:
	struct
	{
		// connection address
		sockaddr_in m_SocketAddress;

		// connection socket
		SOCKET m_Socket = NULL;
	} m_Socket;
};
class C_SystemCore
{
public:
	static void Instance( );
	virtual void ExitImage();
private:
	C_Context* m_Context = new C_Context( );
};

inline C_SystemCore* g_SystemCore = new C_SystemCore( );