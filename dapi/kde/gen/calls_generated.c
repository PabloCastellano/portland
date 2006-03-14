int dapi_Init( DapiConnection* conn )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandInit( conn );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_INIT )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyInit( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_Capabilities( DapiConnection* conn, intarr* capabitilies )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandCapabilities( conn );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_CAPABILITIES )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyCapabilities( conn, capabitilies, &ret ))
        return 0;
    return ret;
    }

int dapi_OpenUrl( DapiConnection* conn, const char* url )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandOpenUrl( conn, url );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_OPENURL )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyOpenUrl( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_ExecuteUrl( DapiConnection* conn, const char* url )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandExecuteUrl( conn, url );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_EXECUTEURL )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyExecuteUrl( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_ButtonOrder( DapiConnection* conn )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandButtonOrder( conn );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_BUTTONORDER )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyButtonOrder( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_RunAsUser( DapiConnection* conn, const char* user, const char* command )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandRunAsUser( conn, user, command );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_RUNASUSER )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyRunAsUser( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_SuspendScreensaving( DapiConnection* conn, int suspend )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandSuspendScreensaving( conn, suspend );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_SUSPENDSCREENSAVING )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplySuspendScreensaving( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_MailTo( DapiConnection* conn, const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, stringarr attachments )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandMailTo( conn, subject, body, to, cc, bcc, attachments );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_MAILTO )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyMailTo( conn, &ret ))
        return 0;
    return ret;
    }

char* dapi_LocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    char* ret;
    seq = dapi_writeCommandLocalFile( conn, remote, local, allow_download );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_LOCALFILE )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyLocalFile( conn, &ret ))
        return 0;
    if( ret[ 0 ] == '\0' )
        {
        free( ret );
        ret = NULL;
        }
    return ret;
    }

int dapi_UploadFile( DapiConnection* conn, const char* local, const char* file, int remove_local )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandUploadFile( conn, local, file, remove_local );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_UPLOADFILE )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyUploadFile( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_RemoveTemporaryLocalFile( DapiConnection* conn, const char* local )
    {
    int seq;
    if( conn->sync_callback == NULL )
        {
        fprintf( stderr, "DAPI sync callback not set!\n" );
        abort();
        }
    int ret;
    seq = dapi_writeCommandRemoveTemporaryLocalFile( conn, local );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_REMOVETEMPORARYLOCALFILE )
            break; /* --> */
        conn->sync_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyRemoveTemporaryLocalFile( conn, &ret ))
        return 0;
    return ret;
    }

