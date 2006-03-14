int dapi_callbackInit( DapiConnection* conn, dapi_Init_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandInit( conn );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_INIT;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackCapabilities( DapiConnection* conn, dapi_Capabilities_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandCapabilities( conn );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_CAPABILITIES;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackOpenUrl( DapiConnection* conn, const char* url, dapi_OpenUrl_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandOpenUrl( conn, url );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_OPENURL;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackExecuteUrl( DapiConnection* conn, const char* url, dapi_ExecuteUrl_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandExecuteUrl( conn, url );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_EXECUTEURL;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackButtonOrder( DapiConnection* conn, dapi_ButtonOrder_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandButtonOrder( conn );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_BUTTONORDER;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackRunAsUser( DapiConnection* conn, const char* user, const char* command,
    dapi_RunAsUser_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandRunAsUser( conn, user, command );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_RUNASUSER;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackSuspendScreensaving( DapiConnection* conn, int suspend, dapi_SuspendScreensaving_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandSuspendScreensaving( conn, suspend );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_SUSPENDSCREENSAVING;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, dapi_MailTo_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandMailTo( conn, subject, body, to, cc, bcc, attachments );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_MAILTO;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, dapi_LocalFile_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandLocalFile( conn, remote, local, allow_download );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_LOCALFILE;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local, dapi_UploadFile_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandUploadFile( conn, local, file, remove_local );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_UPLOADFILE;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackRemoveTemporaryLocalFile( DapiConnection* conn, const char* local,
    dapi_RemoveTemporaryLocalFile_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandRemoveTemporaryLocalFile( conn, local );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_REMOVETEMPORARYLOCALFILE;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

static void genericCallbackDispatch( DapiConnection* conn, DapiCallbackData* data, int command, int seq )
    {
    switch( command )
        {
        case DAPI_REPLY_INIT:
            {
            int ok;
            dapi_readReplyInit( conn, &ok );
            (( dapi_Init_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_CAPABILITIES:
            {
            intarr capabitilies;
            int ok;
            dapi_readReplyCapabilities( conn, &capabitilies, &ok );
            (( dapi_Capabilities_callback ) data->callback )( conn, data->seq, capabitilies, ok );
            break;
            }
        case DAPI_REPLY_OPENURL:
            {
            int ok;
            dapi_readReplyOpenUrl( conn, &ok );
            (( dapi_OpenUrl_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_EXECUTEURL:
            {
            int ok;
            dapi_readReplyExecuteUrl( conn, &ok );
            (( dapi_ExecuteUrl_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_BUTTONORDER:
            {
            int order;
            dapi_readReplyButtonOrder( conn, &order );
            (( dapi_ButtonOrder_callback ) data->callback )( conn, data->seq, order );
            break;
            }
        case DAPI_REPLY_RUNASUSER:
            {
            int ok;
            dapi_readReplyRunAsUser( conn, &ok );
            (( dapi_RunAsUser_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_SUSPENDSCREENSAVING:
            {
            int ok;
            dapi_readReplySuspendScreensaving( conn, &ok );
            (( dapi_SuspendScreensaving_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_MAILTO:
            {
            int ok;
            dapi_readReplyMailTo( conn, &ok );
            (( dapi_MailTo_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_LOCALFILE:
            {
            char* result;
            dapi_readReplyLocalFile( conn, &result );
            (( dapi_LocalFile_callback ) data->callback )( conn, data->seq, result );
            break;
            }
        case DAPI_REPLY_UPLOADFILE:
            {
            int ok;
            dapi_readReplyUploadFile( conn, &ok );
            (( dapi_UploadFile_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        case DAPI_REPLY_REMOVETEMPORARYLOCALFILE:
            {
            int ok;
            dapi_readReplyRemoveTemporaryLocalFile( conn, &ok );
            (( dapi_RemoveTemporaryLocalFile_callback ) data->callback )( conn, data->seq, ok );
            break;
            }
        }
    }
