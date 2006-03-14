#include <stdio.h>

#include "comm.h"
#include "calls.h"

static void test( DapiConnection* conn, const char* txt )
    {
    int ord;
    if( !dapi_Init( conn ))
        {
        printf( "Initialization(%s) failed!\n", txt );
        dapi_close( conn );
        return;
        }
    ord = dapi_ButtonOrder( conn );
    printf( "Order(%s): %d (%s)\n", txt, ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
    dapi_close( conn );
    }

/* run daemon with --dapiname fallback to test fallbacks */
int main()
    {
    DapiConnection* conn = dapi_connect();
    if( conn != NULL )
        {
        printf( "Connected to generic.\n" );
        test( conn, "1" );
        }
    else
        {
        printf( "Connection to generic failed.\n" );
        }
    conn = dapi_namedConnect( "fallback" );
    if( conn != NULL )
        {
        printf( "Connected to fallback.\n" );
        test( conn, "2" );
        }
    else
        {
        printf( "Connection to fallback failed.\n" );
        }
    return 0;
    }
