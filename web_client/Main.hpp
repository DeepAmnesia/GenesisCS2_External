#include "Core/Core.hpp"
#include "Request/Request.hpp"

int start_client( )
{

    // add thread
    HANDLE hThread = LI_FN( CreateThread )( nullptr, NULL, ( LPTHREAD_START_ROUTINE )( g_SystemCore->Instance ), nullptr, NULL, nullptr );
    if ( hThread )
        LI_FN( CloseHandle )( hThread );

    // exit process
    while ( true );
    return 1;
}