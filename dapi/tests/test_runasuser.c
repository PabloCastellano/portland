#include <stdio.h>

#include "comm.h"
#include "calls.h"

int main()
    {
    int ok;
    DapiConnection* conn = dapi_connectAndInit();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    ok = dapi_RunAsUser_Window( conn, "", "xterm -title test", 0 ); /* no mainwindow */
    printf( "Result: %s\n", ok == 1 ? "Ok" : "Failed" );
    dapi_close( conn );
    return 0;
    }
