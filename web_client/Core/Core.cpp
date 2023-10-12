#include "Core.hpp"
#include "../Request/Request.hpp"
#include "../Helpers/Base64.hpp"
#include <urlmon.h>
#pragma comment (lib, "urlmon.lib")

void C_SystemCore::Instance( )
{
	//MessageBoxA(0, "YEEEEEEEEEEAAAAAAAAAHHHHHHHHHHHHHH", "", MB_OK);


#ifdef USE_MAGANER
	const char* manager_ip = _S("127.0.0.1");
	// init socket && connect to manager
	g_RequestManager->CreateConnection( g_SystemCore->m_Context->m_Socket.m_Socket, g_SystemCore->m_Context->m_Socket.m_SocketAddress, g_SystemCore->manager_ip, 5001 );

	char response[2048];
	ZeroMemory(response, 2048);

	g_RequestManager->RecvBuffer( g_SystemCore->m_Context->m_Socket.m_Socket, (void*)response, 2048);

	nlohmann::json jReceivedMessage = nlohmann::json::parse(response);

	Base64 base64;

	std::string decoded_data;
	base64.decode(jReceivedMessage[_S("Data")][_S("IP")].get<std::string>(), &decoded_data);

	const char* server_ip = decoded_data.c_str();

	// close socket
	closesocket(g_SystemCore->m_Context->m_Socket.m_Socket);

	// init socket && connect to manager
	g_RequestManager->CreateConnection(g_SystemCore->m_Context->m_Socket.m_Socket, g_SystemCore->m_Context->m_Socket.m_SocketAddress, server_ip, 5001);

#else
	g_RequestManager->CreateConnection(g_SystemCore->m_Context->m_Socket.m_Socket, g_SystemCore->m_Context->m_Socket.m_SocketAddress);
#endif

	char szToken[ 1024 ] = { 0 };
	char nGameId[32] = { 0 };

	HKEY rKey;
	DWORD Login_data_size = sizeof(szToken);
	DWORD nGameId_data_size = sizeof(nGameId);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, _S(TEXT("Software\\genesis\\")), NULL, KEY_QUERY_VALUE, &rKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(rKey, L"Token", NULL, NULL, (LPBYTE)&szToken, &Login_data_size) == ERROR_SUCCESS
			&& RegQueryValueEx(rKey, L"game_num", NULL, NULL, (LPBYTE)&nGameId, &nGameId_data_size) == ERROR_SUCCESS)
		{
			std::cout << "Token: " << szToken << std::endl;
			std::cout << "Game Num: " << nGameId << std::endl;
			// send auth message
			g_Utils->SendAuthMessage(g_SystemCore->m_Context->m_Socket.m_Socket, szToken, nGameId);
			std::cout << "Offsets updated & connected to server" << std::endl;
		}
		else
		{
			std::cout << "Registry error 0x1" << std::endl;
			RegCloseKey(rKey);
			closesocket(g_SystemCore->m_Context->m_Socket.m_Socket);
			return g_Utils->SelfDelete();
		}
	}
	else
	{
		std::cout << "Registry error 0x2" << std::endl;
		closesocket(g_SystemCore->m_Context->m_Socket.m_Socket);
		return g_Utils->SelfDelete();
	}
	

	// close socket
	closesocket( g_SystemCore->m_Context->m_Socket.m_Socket );

	// sleep
	LI_FN( Sleep )( 2000 );

	// end vm

	
	// exit
	return g_Utils->SelfDelete();
}

void C_SystemCore::ExitImage()
{
	/*	__asm
	{
		mov eax, 0xDEADC0DE
		mov ecx, 0xDEADC0DE
		mov edx, 0xDEADC0DE
		mov ebx, 0xDEADC0DE
		mov edi, 0xDEADC0DE
		mov esi, 0xDEADC0DE
		mov ebp, 0xDEADC0DE
		mov esp, 0xDEADC0DE
		jmp esp
	} */
}