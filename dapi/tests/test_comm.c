#include <stdio.h>

#include "comm.h"

int main()
    {
    int command, seq, seq2;
    int ok;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    seq = dapi_writeCommandInit( conn );
    if( !dapi_readCommand( conn, &command, &seq2 ) || seq != seq2 )
        {
        fprintf( stderr, "Incorrect init reply!\n" );
        return 2;
        }
    if( !dapi_readReplyInit( conn, &ok ))
        {
        fprintf( stderr, "Incorrect init reply data!\n" );
        return 2;
        }
    if( !ok )
        {
        fprintf( stderr, "Initialization failed!\n" );
        return 2;
        }
    seq = dapi_writeCommandOpenUrl( conn, "http://kde.org" );
    if( !dapi_readCommand( conn, &command, &seq2 ) || seq != seq2 )
        {
        fprintf( stderr, "Incorrect open url reply!\n" );
        return 2;
        }
    if( !dapi_readReplyOpenUrl( conn, &ok ))
        {
        fprintf( stderr, "Incorrect open url reply data!\n" );
        return 2;
        }
    printf( "Result: %s\n", ok ? "Ok" : "failed" );
    dapi_close( conn );
    return 0;
    }
