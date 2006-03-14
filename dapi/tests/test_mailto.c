#include <stdio.h>
#include <stdlib.h>

#include <dapi/comm.h>
#include <dapi/calls.h>

int main()
    {
    int ok;
    stringarr attachments;
    char* attachments_data[] = { "/tmp/mailtotest.txt" };
    DapiConnection* conn = dapi_connectAndInit();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    system( "touch /tmp/mailtotest.txt" );
    attachments.count = 1;
    attachments.data = attachments_data;
    ok = dapi_MailTo_Window( conn, "Test mail", "Hi,\n\nthis is a test mail.\n",
        "l.lunak@suse.cz, l.lunak@kde.org", NULL, "portland@lists.freedesktop.org", attachments,
        0 ); /* no mainwindow */
    printf( "Result: %s\n", ok == 1 ? "Ok" : "Failed" );
    dapi_close( conn );
    return 0;
    }
