#ifndef DAPI_COMM_H
#define DAPI_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DapiConnection DapiConnection;

DapiConnection* dapi_connect( void );
DapiConnection* dapi_namedConnect( const char* name );
void dapi_close( DapiConnection* conn );
int dapi_socket( DapiConnection* conn );

DapiConnection* dapi_connectAndInit( void );
DapiConnection* dapi_namedConnectAndInit( const char* name );

int dapi_bindSocket( void );
int dapi_namedBindSocket( const char* name );
DapiConnection* dapi_acceptSocket( int sock );

typedef struct DapiWindowInfo
    {
    int flags;
    long window;
    } DapiWindowInfo;

/* TODO generovat? */
typedef struct
    {
    int count;
    int* data;
    } intarr;
    
typedef struct
    {
    int count;
    char** data;
    } stringarr;

void dapi_windowInfoInitWindow( DapiWindowInfo* winfo, long window );

void dapi_freeWindowInfo( DapiWindowInfo winfo );
void dapi_freestringarr( stringarr arr );
void dapi_freeintarr( intarr arr );

#include <dapi/comm_generated.h>

#ifdef __cplusplus
}
#endif

#endif
