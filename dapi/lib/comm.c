#include "comm.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "comm_internal.h"

static void getDisplay( char* ret, int max )
    {
    char hostname[ 256 ];
    const char* disp = getenv( "DISPLAY" );
    char* pos;
    *ret = '\0';
    if( disp == NULL )
        disp = ":0";
    if( *disp == ':' )
        {
        gethostname( ret, max );
        hostname[ max - 1 ] = '\0';
        }
    strncat( ret, disp, max - 1 );
    pos = strrchr( ret, '.' ); /* strip head */
    if( pos != NULL && pos > strrchr( ret, ':' ))
        *pos = '\0';
    }

static void socketName( char* name, int max )
    {
    const char* home;
    char display[ 256 ];
    home = getenv( "HOME" );
    getDisplay( display, 255 );
    snprintf( name, max - 1, "%s/.dapi-%s", home, display );
    }

DapiConnection* dapi_connect()
    {
    char sock_file[ 256 ];
    int sock;
    struct sockaddr_un addr;
    DapiConnection* ret;
    socketName( sock_file, 255 );
    sock = socket( PF_UNIX, SOCK_STREAM, 0 );
    if( sock < 0 )
        {
        perror( "socket" );
        return NULL;
        }
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, sock_file );
    if( connect( sock, ( struct sockaddr* ) &addr, sizeof( addr )) < 0 )
        {
        perror( "connect" );
        close( sock );
        return NULL;
        }
    ret = malloc( sizeof( DapiConnection ));
    if( ret != NULL )
        {
        ret->sock = sock;
        ret->sync_callback = NULL;
        ret->in_server = 0;
        ret->last_seq = 0;
        }
    return ret;
    }

int dapi_bindSocket()
    {
    char sock_file[ 256 ];
    int sock;
    struct sockaddr_un addr;
    socketName( sock_file, 255 );
    sock = socket( PF_UNIX, SOCK_STREAM, 0 );
    if( sock < 0 )
        {
        perror( "socket" );
        return -1;
        }
    int opt = fcntl( sock, F_GETFL );
    if( opt < 0 )
        {
        perror( "fcntl" );
        close( sock );
        return -1;
        }
    if( fcntl( sock, F_SETFL, opt | O_NONBLOCK ) < 0 )
        {
        perror( "nonblock" );
        close( sock );
        return -1;
        }
    unlink( sock_file );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, sock_file );
    if( bind( sock, ( struct sockaddr* ) &addr, sizeof( addr )) < 0 )
        {
        perror( "bind" );
        close( sock );
        return -1;
        }
    if( chmod( sock_file, 0600 ) != 0 )
        {
        perror( "chmod" );
        close( sock );
        return -1;
        }
    if( listen( sock, SOMAXCONN ) < 0 )
        {
        perror( "listen" );
        close( sock );
        return -1;
        }
    return sock;
    }

int dapi_socket( DapiConnection* conn )
    {
    return conn->sock;
    }

static int writeSocket( DapiConnection* conn, const void* data, int size )
    {
    int written = 0;
    for(;;)
        {
        int len = write( conn->sock, ( const char* ) data + written, size - written );
        if( len < 0 && errno != EINTR && errno != EAGAIN )
            return -1;
        if( len > 0 )
            written += len;
        if( written == size )
            return 1;
        }
    }

static int readSocket( DapiConnection* conn, void* data, int size )
    {
    int rd = 0;
    for(;;)
        {
        int len = read( conn->sock, ( char* ) data + rd, size - rd );
        if( len < 0 && errno != EINTR && errno != EAGAIN )
            return -1;
        if( len == 0 )
            return 0;
        if( len > 0 )
            rd += len;
        if( rd == size )
            return 1;
        }
    }

DapiConnection* dapi_acceptSocket( int sock )
    {
    struct sockaddr_un addr;
    DapiConnection* ret = NULL;
    socklen_t addr_len = sizeof( addr );
    int sock2 = accept( sock, ( struct sockaddr* ) &addr, &addr_len );
    if( sock2 > 0 )
        {
        ret = malloc( sizeof( DapiConnection ));
        if( ret != NULL )
            {
            ret->sock = sock2;
            ret->sync_callback = NULL;
            ret->in_server = 1;
            ret->last_seq = 0;
            }
        else
            close( sock2 );
        }
    return ret;
    }

void dapi_close( DapiConnection* conn )
    {
    close( conn->sock );
    }

static int getNextSeq( DapiConnection* conn )
    {
    if( ++conn->last_seq == 0 ) // 0 means invalid
        ++conn->last_seq;
    return conn->last_seq;
    }

static char* readString( DapiConnection* conn )
    {
    int len;
    if( readSocket( conn, &len, sizeof( len )) <= 0 )
        return NULL;
    char* ret = malloc( len + 1 );
    if( ret == NULL )
        return NULL;
    if( len > 0 )
        {
        if( readSocket( conn, ret, len ) <= 0 )
            {
            free( ret );
            return NULL;
            }
        }
    ret[ len ] = '\0';
    return ret;
    }

static void writeString( DapiConnection* conn, const char* str )
    {
    int len = ( str == NULL ? 0 : strlen( str ));
    writeSocket( conn, &len, sizeof( len ));
    if( len > 0 )
        writeSocket( conn, str, len );
    }

void dapi_freeStringList( char** strs )
    {
    if( strs != NULL )
        {
        char** pos;
        for( pos = strs;
             *pos != NULL;
             ++pos )
            free( *pos );
        free( strs );
        }
    }

static char** readStringList( DapiConnection* conn )
    {
    int count;
    char** ret;
    int i;
    if( readSocket( conn, &count, sizeof( count )) <= 0 )
        return NULL;
    ret = malloc(( count + 1 ) * sizeof( char** ));
    if( ret == NULL )
        return NULL;
    for( i = 0;
         i < count;
         ++i )
        ret[ i ] = NULL;
    for( i = 0;
         i < count;
         ++i )
        {
        ret[ i ] = readString( conn );
        if( ret[ i ] == NULL )
            {
            dapi_freeStringList( ret );
            return NULL;
            }
        }
    ret[ count ] = NULL; /* terminating NULL */
    return ret;
    }

static void writeStringList( DapiConnection* conn, const char** strs )
    {
    int count = 0;
    const char** pos;
    if( strs == NULL || *strs == NULL )
        {
        int count = 0;
        writeSocket( conn, &count, sizeof( count ));
        return;
        }
    for( pos = strs;
         *pos != NULL;
         ++pos )
        ++count;
    writeSocket( conn, &count, sizeof( count ));
    for( pos = strs;
         *pos != NULL;
         ++pos )
        writeString( conn, *pos );
    }

int dapi_readCommand( DapiConnection* conn, int* comm, int* seq )
    {
    int magic;
    /* TODO kontrola failure */
    readSocket( conn, &magic, sizeof( magic ));
    readSocket( conn, comm, sizeof( *comm ));
    readSocket( conn, seq, sizeof( *seq ));
    if( magic != MAGIC )
        return 0;
    return 1;
    }

static void writeCommand( DapiConnection* conn, int comm, int seq )
    {
    int magic = MAGIC;
    writeSocket( conn, &magic, sizeof( magic ));
    writeSocket( conn, &comm, sizeof( comm ));
    writeSocket( conn, &seq, sizeof( seq ));
    }

#include "../kde/gen/comm_generated.c"
