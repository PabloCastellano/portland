#include <stdio.h>
#include <unistd.h>

#include <dapi/comm.h>
#include <dapi/calls.h>

int main()
    {
    int ok;
    DapiConnection* conn = dapi_connectAndInit();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    ok = dapi_SuspendScreensaving( conn, 1 );
    printf( "Result1: %s\n", ok == 1 ? "Ok" : "Failed" );
    sleep( 10 );
    ok = dapi_SuspendScreensaving( conn, 0 );
    printf( "Result2: %s\n", ok == 1 ? "Ok" : "Failed" );
    dapi_close( conn );
    return 0;
    }
