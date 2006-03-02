#include "calls.h"

#include <stdio.h>
#include <stdlib.h>

#include "comm.h"
#include "comm_internal.h"

#include "../kde/gen/calls_generated.c"

void dapi_setSyncCallback( DapiConnection* conn, DapiSyncCallback callback )
    {
    conn->sync_callback = callback;
    }
