#include <stdio.h>
#include <unistd.h>

#include "comm.h"
#include "calls.h"

int main()
    {
    int ok;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    if( !dapi_Init( conn ))
        {
        fprintf( stderr, "Initialization failed!\n" );
        return 2;
        }
    ok = dapi_SuspendScreensaving( conn, 1 );
    printf( "Result1: %s\n", ok == 1 ? "Ok" : "Failed" );
    sleep( 10 );
    ok = dapi_SuspendScreensaving( conn, 0 );
    printf( "Result2: %s\n", ok == 1 ? "Ok" : "Failed" );
    dapi_close( conn );
    return 0;
    }
