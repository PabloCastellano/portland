#include <stdio.h>

#include <dapi/comm.h>
#include <dapi/calls.h>

int main()
    {
    int ord;
    DapiConnection* conn = dapi_connectAndInit();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    ord = dapi_ButtonOrder( conn );
    printf( "Order: %d (%s)\n", ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
    dapi_close( conn );
    return 0;
    }
