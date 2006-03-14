#ifndef DAPI_CALLBACKS_H
#define DAPI_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "comm.h"

#include "../kde/gen/callbacks_generated.h"

typedef void (*DapiGenericCallback)( DapiConnection* conn, int command, int seq );

DapiGenericCallback dapi_setGenericCallback( DapiConnection* conn, DapiGenericCallback callback );

void dapi_processCallbacks( DapiConnection* conn );

void dapi_genericCallback( DapiConnection* conn, int command, int seq );

#ifdef __cplusplus
}
#endif

#endif
