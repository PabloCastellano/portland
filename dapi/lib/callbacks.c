#include "callbacks.h"

#include <stdio.h>
#include <stdlib.h>

#include "comm_internal.h"

#include "../kde/gen/callbacks_generated.c"

DapiGenericCallback dapi_setGenericCallback( DapiConnection* conn, DapiGenericCallback callback )
    {
    DapiGenericCallback ret = conn->generic_callback;
    conn->generic_callback = callback;
    return ret;
    }

void dapi_processCallbacks( DapiConnection* conn )
    {
    while( dapi_hasData( conn ))
        {
        int command;
        int seq;
        if( !dapi_readCommand( conn, &command, &seq ))
            return; /* TODO error-handling? */
        conn->generic_callback( conn, command, seq );
        }
    }

void dapi_genericCallback( DapiConnection* conn, int command, int seq )
    {
    DapiCallbackData* pos;
    DapiCallbackData* prev = NULL;
    for( pos = conn->callbacks;
         pos != NULL;
         prev = pos, pos = pos->next )
        {
        if( pos->seq == seq )
            {
            if( pos->callback != NULL )
                genericCallbackDispatch( conn, pos, command, seq );
            if( prev != NULL )
                prev = pos->next;
            else
                conn->callbacks = pos->next;
            free( pos );
            return;
            }
        }
/* TODO handle unhandled */
    }
