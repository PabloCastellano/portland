enum { MAGIC = 0x152355 };

#include "../kde/gen/comm_internal_generated.h"

#include "calls.h"
#include "callbacks.h"

typedef struct DapiCallbackData
    {
    struct DapiCallbackData* next;
    int seq;
    int command;
    void* callback;
    } DapiCallbackData;

struct DapiConnection
    {
    int sock;
    DapiGenericCallback generic_callback;
    int in_server;
    int last_seq;
    DapiCallbackData* callbacks;
    };

int dapi_hasData( DapiConnection* conn );
