#ifndef DAPI_COMM_H
#define DAPI_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DapiConnection DapiConnection;

DapiConnection* dapi_connect( void );
void dapi_close( DapiConnection* conn );
int dapi_socket( DapiConnection* conn );

int dapi_bindSocket( void );
DapiConnection* dapi_acceptSocket( int sock );

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

void dapi_freestringarr( stringarr arr );
void dapi_freeintarr( intarr arr );

#include "../kde/gen/comm_generated.h"

#ifdef __cplusplus
}
#endif

#endif
