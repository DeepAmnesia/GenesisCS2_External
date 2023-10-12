#include "Utils.hpp"
#include "../Core/Core.hpp"
#include "../Request/Request.hpp"
#include "../Helpers/XorStr.hpp"
#include "../MD5/MD5.hpp"
#include "../Helpers/Base64.hpp"
#include "../../classes/globals.hpp"

void C_Utils::SendAuthMessage( SOCKET m_Socket, const char* szToken, char GameId[32])
{
	// setup message
	nlohmann::json jAuthMessage;

	std::cout << "Compiling request data..." << std::endl;

	jAuthMessage[_S("Data")][_S("ID")] = MessageType::LoaderAuth;
	jAuthMessage[_S("Data")][_S("Token")] = szToken;
	jAuthMessage[_S("Data")][_S("HWID")] = this->GetHWID();
	jAuthMessage[_S("Data")][_S("GameId")] = GameId;
	jAuthMessage[_S("Data")][_S("Version")] = _S("amFzZGhla3d3");

	// buffer
	std::string szBuffer = jAuthMessage.dump();
	std::cout << "Sending buffer... size: " << szBuffer.length() << std::endl;

	g_RequestManager->SendBuffer( m_Socket, (void*)szBuffer.c_str(), szBuffer.length());

	std::cout << "Request sent" << std::endl;

	//get offsets list
	char response[2048];
	ZeroMemory(response, 2048);
	g_RequestManager->RecvBuffer(m_Socket, (void*)response, 2048);

	std::cout << "Got server response" << std::endl;

	nlohmann::json jReceivedMessage;

	std::cout << "Fetching request..." << std::endl;

	if (nlohmann::json::accept(response))
	{
		jReceivedMessage = nlohmann::json::parse(response);
		if (!jReceivedMessage["Data"].is_null() &&
			jReceivedMessage["Data"].is_object() &&
			jReceivedMessage["Data"]["Result"].get<std::string>() == "Success" &&
			!jReceivedMessage["Offsets"].is_null() &&
			jReceivedMessage["Offsets"].is_object()
			)
		{
			std::cout << "Request fetched" << std::endl;
			std::cout << "Setupping offsets..." << std::endl;

			/*config::dwLocalPlayer = jReceivedMessage["Offsets"]["dwLocalPlayer"].get<DWORD>();
			config::dwEntityList = jReceivedMessage["Offsets"]["dwEntityList"].get<DWORD>();
			config::dwViewMatrix = jReceivedMessage["Offsets"]["dwViewMatrix"].get<DWORD>();
			config::dwPawnHealth = jReceivedMessage["Offsets"]["dwPawnHealth"].get<DWORD>();
			config::dwPlayerPawn = jReceivedMessage["Offsets"]["dwPlayerPawn"].get<DWORD>();
			config::dwSanitizedName = jReceivedMessage["Offsets"]["dwSanitizedName"].get<DWORD>();
			config::m_iTeamNum = jReceivedMessage["Offsets"]["m_iTeamNum"].get<DWORD>();
			config::m_vecOrigin = jReceivedMessage["Offsets"]["m_vecOrigin"].get<DWORD>();*/
			g::username = jReceivedMessage["Data"]["Username"].get<std::string>();

			std::cout << "Offsets setupped" << std::endl;
		}
		else
		{
			MessageBoxA(0, "0x1 Server auth error\nMaybe, you renamed loader file", "Genesis", MB_OK);
			g_Utils->SelfDelete();
		}
	}
	else
	{
		MessageBoxA(0, "0x2 Server auth error\nMaybe, you renamed loader file", "Genesis", MB_OK);
		g_Utils->SelfDelete();
	}

}

void C_Utils::AskConfigList( SOCKET m_Socket )
{


		// setup message
	nlohmann::json jMessage;

	jMessage[_S("Data")][_S("ID")] = MessageType::GetConfigList;
	jMessage[_S("Data")][_S("HWID")] = this->GetHWID();

	// buffer
	std::string aBuffer = jMessage.dump();

	char response[2048];
	ZeroMemory(response, 2048);

	// send message
	g_RequestManager->SendBuffer(m_Socket, (void*)aBuffer.c_str( ), aBuffer.length( ));

	g_RequestManager->RecvBuffer(m_Socket, (void*)response, 2048);

	MessageBoxA(0, response, "That is all", MB_OK);
}

void C_Utils::AskConfig(SOCKET m_Socket, int id)
{

	// setup message
	nlohmann::json jMessage;

	jMessage[_S("Data")][_S("ID")] = MessageType::GetConfig;
	jMessage[_S("Data")][_S("ConfigId")] = id;
	jMessage[_S("Data")][_S("HWID")] = this->GetHWID();

	// buffer
	std::string aBuffer = jMessage.dump();

	char response[2048];
	ZeroMemory(response, 2048);

	// send message
	g_RequestManager->SendBuffer(m_Socket, (void*)aBuffer.c_str(), aBuffer.length());

	g_RequestManager->RecvBuffer(m_Socket, (void*)response, 2048);

	MessageBoxA(0, response, "That is all", MB_OK);
}

std::string C_Utils::GetHWID( )
{
	HANDLE hFile = CreateFileA( _S( "\\\\.\\PhysicalDrive0" ), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
		return { };

	std::unique_ptr< std::remove_pointer <HANDLE >::type, void(*)(HANDLE) > hDevice
	{
		hFile, [ ]( HANDLE handle )
		{
			CloseHandle( handle );
		}
	};

	STORAGE_PROPERTY_QUERY PropertyQuery;
	PropertyQuery.PropertyId = StorageDeviceProperty;
	PropertyQuery.QueryType = PropertyStandardQuery;

	STORAGE_DESCRIPTOR_HEADER DescHeader;
	DWORD dwBytesReturned = 0;
	if ( !DeviceIoControl( hDevice.get( ), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof( STORAGE_PROPERTY_QUERY ),
		&DescHeader, sizeof( STORAGE_DESCRIPTOR_HEADER ), &dwBytesReturned, NULL ) )
		return { };

	const DWORD dwOutBufferSize = DescHeader.Size;
	std::unique_ptr< BYTE[] > pOutBuffer{ new BYTE[ dwOutBufferSize ] { } };

	if ( !DeviceIoControl( hDevice.get( ), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof( STORAGE_PROPERTY_QUERY ),
		pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL ) )
		return { };

	STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast< STORAGE_DEVICE_DESCRIPTOR* >( pOutBuffer.get( ) );
	if ( !pDeviceDescriptor )
		return { };

	const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
	if ( !dwSerialNumberOffset)
		return { };

	std::string sResult = reinterpret_cast< const char* >( pOutBuffer.get() + dwSerialNumberOffset );
	sResult.erase( std::remove_if( sResult.begin( ), sResult.end (), std::isspace ), sResult.end( ) );
	return sResult;
}

void C_Utils::SelfDelete()
{
	TCHAR szModuleName[MAX_PATH];
	TCHAR szCmd[2 * MAX_PATH];
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	GetModuleFileName(NULL, szModuleName, MAX_PATH);

	StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

	CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	exit(-1);
}