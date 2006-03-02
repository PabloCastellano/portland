#include <stdio.h>

#include "comm.h"
#include "calls.h"

static int seq1;

static void callback( DapiConnection* conn, int comm, int seq )
    {
    if( comm != DAPI_REPLY_BUTTONORDER
        || seq != seq1 )
        {
        fprintf( stderr, "Unexpected async reply, ignoring.\n" );
        }
    else
        {
        int ord;
        if( !dapi_readReplyButtonOrder( conn, &ord ))
            {
            fprintf( stderr, "Failed to read async reply!\n" );
            }
        else
            {
            printf( "Order async: %d (%s)\n", ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
            }
        }
    }

int main()
    {
    int ord;
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
    /* first write a command request without waiting for a reply, this
       means that the following blocking dapi_ButtonOrder() call will first need to process
       an async answer */
    seq1 = dapi_writeCommandButtonOrder( conn );
    ord = dapi_ButtonOrder( conn );
    printf( "Order: %d (%s)\n", ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
    dapi_close( conn );
    return 0;
    }
