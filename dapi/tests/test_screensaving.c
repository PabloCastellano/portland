#include <stdio.h>
#include <unistd.h>

#include "comm.h"
#include "calls.h"

static void callback( DapiConnection* a1, int a2, int a3 )
    {
    (void) a1;
    (void) a2;
    (void) a3;
    fprintf( stderr, "Unexpected async reply, ignoring.\n" );
    }

int main()
    {
    int ok;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    dapi_setSyncCallback( conn, callback );
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
