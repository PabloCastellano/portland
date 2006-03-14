#include <stdio.h>

#include <dapi/comm.h>
#include <dapi/calls.h>

static void test( DapiConnection* conn, const char* txt )
    {
    int ord = dapi_ButtonOrder( conn );
    printf( "Order(%s): %d (%s)\n", txt, ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
    dapi_close( conn );
    }

/* run daemon with --dapiname fallback to test fallbacks */
int main()
    {
    DapiConnection* conn = dapi_connectAndInit();
    if( conn != NULL )
        {
        printf( "Connected to generic.\n" );
        test( conn, "1" );
        }
    else
        {
        printf( "Connection to generic failed.\n" );
        }
    conn = dapi_namedConnectAndInit( "fallback" );
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
