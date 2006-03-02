#include <stdio.h>
#include <stdlib.h>

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
    const char* attachments[] = { "/tmp/mailtotest.txt", NULL };
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
    system( "touch /tmp/mailtotest.txt" );
    ok = dapi_MailTo( conn, "Test mail", "Hi,\n\nthis is a test mail.\n",
        "l.lunak@suse.cz, l.lunak@kde.org", NULL, "portland@lists.freedesktop.org", attachments );
    printf( "Result: %s\n", ok == 1 ? "Ok" : "Failed" );
    dapi_close( conn );
    return 0;
    }
