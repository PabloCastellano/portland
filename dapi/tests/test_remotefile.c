#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "comm.h"
#include "calls.h"

int main()
    {
    char* local;
    int ok;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    if( !dapi_Init( conn ))
        {
        fprintf( stderr, "Initialization failed!\n" );
        return 2;
        }
    local = dapi_LocalFile_Window( conn, "http://kde.org", "",  0, 0 ); /* no download, should fail */
    printf( "Local file1: %s - %s\n", local ? local : "?", local ? "Failed" : "Ok" );
    if( local )
        free( local );
    local = dapi_LocalFile_Window( conn, "http://kde.org", "", 1, 0 );
    printf( "Local file2: %s\n", local != NULL ? local : "Failed" );
    if( local != NULL )
        {
        ok = dapi_UploadFile_Window( conn, local, "http://kde.org", 0, 0 ); /* will fail */
        printf( "Upload2: %s\n", ok ? "Ok - ???" : "Failed - ok" );
        ok = dapi_RemoveTemporaryLocalFile( conn, local );
        printf( "Temporary2: %s\n", ok ? "Ok" : "Failed" );
        free( local );
        }
    system( "touch /tmp/remotefiletest.txt" );
    /* local temporary will be ignored */
    local = dapi_LocalFile_Window( conn, "file:///tmp/remotefiletest.txt", "/tmp/remotefiletest2.txt", 1, 0 );
    printf( "Local file3: %s\n", local != NULL ? local : "Failed" );
    if( local != NULL )
        {
        /* should be a no-op, as it's the same file */
        ok = dapi_UploadFile_Window( conn, local, "file:///tmp/remotefiletest.txt", 1, 0 );
        printf( "Upload3: %s\n", ok ? "Ok" : "Failed" );
        free( local );
        }
    dapi_close( conn );
    return 0;
    }
