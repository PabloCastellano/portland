enum { MAGIC = 0x152355 };

#include "../kde/gen/comm_internal_generated.h"

#include "calls.h"

struct DapiConnection
    {
    int sock;
    DapiSyncCallback sync_callback;
    int in_server;
    int last_seq;
    };
