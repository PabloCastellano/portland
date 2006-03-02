#ifndef DAPI_CALLS_H
#define DAPI_CALLS_H

#include "comm.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../kde/gen/calls_generated.h"

typedef void (*DapiSyncCallback)( DapiConnection* conn, int command, int seq );

void dapi_setSyncCallback( DapiConnection* conn, DapiSyncCallback callback );

#ifdef __cplusplus
}
#endif

#endif
