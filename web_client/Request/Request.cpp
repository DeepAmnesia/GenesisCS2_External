#include "Request.hpp"
#include "../Helpers/FN1VA.hpp"
#include "../Core/Core.hpp"
#include "../MD5/MD5.hpp"
#include "../Helpers/Base64.hpp"


void C_RequestManager::CreateConnection( SOCKET& m_Socket, sockaddr_in& m_SocketAddress)
{

	// init WSA
	WSADATA WSAData;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &WSAData ) != 0 )
		return;

	// create a socket
	m_Socket = ( int )( socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) );
	if ( m_Socket == INVALID_SOCKET )
	{
		MessageBoxA(0, _S("Connection to server error 0x1\nContact to support"), _S("Genesis"), MB_ICONERROR | MB_OK);

		// exit image
		return g_Utils->SelfDelete();
	}


	// setup address
	m_SocketAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	m_SocketAddress.sin_family = AF_INET;
	m_SocketAddress.sin_port = htons(5001);

	// setup ip
	inet_pton(AF_INET, "127.0.0.1", &m_SocketAddress.sin_addr);
	
	// connect to the server
	if ( connect( ( SOCKET )( m_Socket ), ( sockaddr* )( &m_SocketAddress ), sizeof( sockaddr_in ) ) == SOCKET_ERROR )
	{
		MessageBoxA(0, _S("Connection to server error 0x2\nContact to support"), _S("Genesis"), MB_ICONERROR | MB_OK);

		// exit image
		return g_Utils->SelfDelete();
	}

}
int C_RequestManager::SendBuffer( SOCKET m_Socket, void* Buffer, int nSendLength )
{


	int nBytesSent = 0;
	while ( nBytesSent < nSendLength )
	{
		int nSentSize = send( m_Socket, ( const char* )( ( DWORD )( Buffer ) + nBytesSent ), nSendLength - nBytesSent, NULL );
		if ( nSentSize <= -1 )
		{
			int nLastError = WSAGetLastError( );
			if ( nLastError == WSAECONNABORTED )
				MessageBoxA(0, _S("Established connection was aborted by the software in your host computer. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if ( nLastError == WSAECONNREFUSED )
				MessageBoxA(0, _S("No connection could be made because the target machine actively refused it. Plase, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if ( nLastError == WSAECONNRESET )
				MessageBoxA(0, _S("Connection was closed by the remote host. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if ( nLastError == WSAETIMEDOUT )
				MessageBoxA(0, _S("Connection timed out. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			
			// sleep
			LI_FN( Sleep )( 4000 );

			// close socket
			closesocket( m_Socket );

			// exit image
			g_Utils->SelfDelete();

			// get out of the cycle due to server error
			break;
		}

		nBytesSent += nSentSize;
	}



	return nBytesSent;
}
int C_RequestManager::RecvBuffer( SOCKET m_Socket, void* Buffer, int nMaxSize )
{


	// clean out the fucking buffer
	ZeroMemory( Buffer, nMaxSize );

	// run cycle
	int nBytesReceived = 0;
	while ( true )
	{
		int nRecvSize = recv( m_Socket, ( PCHAR )( ( DWORD )( Buffer ) + nBytesReceived ), nMaxSize - nBytesReceived, 0 );
		if ( nRecvSize <= 0 )
		{
			int nLastError = WSAGetLastError( );
			if ( nLastError == WSAECONNABORTED )
				MessageBoxA(0, _S("Established connection was aborted by the software in your host computer. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if (nLastError == WSAECONNREFUSED)
				MessageBoxA(0, _S("No connection could be made because the target machine actively refused it. Plase, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if (nLastError == WSAECONNRESET)
				MessageBoxA(0, _S("Connection was closed by the remote host. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else if (nLastError == WSAETIMEDOUT)
				MessageBoxA(0, _S("Connection timed out. Please, try again later.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);
			else
				MessageBoxA(0, _S("Connection was remotely closed.\n"), _S("Genesis"), MB_ICONERROR | MB_OK);

			// sleep
			LI_FN( Sleep )( 4000 );

			// close socket
			closesocket( m_Socket );

			// exit image
			g_Utils->SelfDelete();

			// get out of the cycle due to server error
			break;
		}

		nBytesReceived += nRecvSize;
		if ( nlohmann::json::accept( ( char* )( Buffer ) ) || nMaxSize == nBytesReceived )
			break;
	}

		
	return nBytesReceived;
}