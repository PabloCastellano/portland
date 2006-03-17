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

int dapi_callbackOpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo,
    dapi_OpenUrl_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandOpenUrl( conn, url, winfo );
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

int dapi_callbackExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo,
    dapi_ExecuteUrl_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandExecuteUrl( conn, url, winfo );
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
    DapiWindowInfo winfo, dapi_RunAsUser_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandRunAsUser( conn, user, command, winfo );
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
    const char* to, const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo,
    dapi_MailTo_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandMailTo( conn, subject, body, to, cc, bcc, attachments, winfo );
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
    int allow_download, DapiWindowInfo winfo, dapi_LocalFile_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandLocalFile( conn, remote, local, allow_download, winfo );
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
    int remove_local, DapiWindowInfo winfo, dapi_UploadFile_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandUploadFile( conn, local, file, remove_local, winfo );
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

int dapi_callbackAddressBookList( DapiConnection* conn, dapi_AddressBookList_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookList( conn );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKLIST;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackAddressBookGetName( DapiConnection* conn, const char* id, dapi_AddressBookGetName_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookGetName( conn, id );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKGETNAME;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackAddressBookGetEmails( DapiConnection* conn, const char* id, dapi_AddressBookGetEmails_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookGetEmails( conn, id );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKGETEMAILS;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackAddressBookFindByName( DapiConnection* conn, const char* name, dapi_AddressBookFindByName_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookFindByName( conn, name );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKFINDBYNAME;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackAddressBookOwner( DapiConnection* conn, dapi_AddressBookOwner_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookOwner( conn );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKOWNER;
    call->next = conn->callbacks;
    conn->callbacks = call;
    return seq;
    }

int dapi_callbackAddressBookGetVCard30( DapiConnection* conn, const char* id, dapi_AddressBookGetVCard30_callback callback )
    {
    int seq;
    DapiCallbackData* call;
    seq = dapi_writeCommandAddressBookGetVCard30( conn, id );
    if( seq == 0 )
        return 0;
    call = malloc( sizeof( *call ));
    if( call == NULL )
        return 0;
    call->seq = seq;
    call->callback = callback;
    call->command = DAPI_COMMAND_ADDRESSBOOKGETVCARD30;
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
        case DAPI_REPLY_ADDRESSBOOKLIST:
            {
            stringarr idlist;
            int ok;
            dapi_readReplyAddressBookList( conn, &idlist, &ok );
            (( dapi_AddressBookList_callback ) data->callback )( conn, data->seq, idlist, ok );
            break;
            }
        case DAPI_REPLY_ADDRESSBOOKGETNAME:
            {
            char* givenname;
            char* familyname;
            char* fullname;
            int ok;
            dapi_readReplyAddressBookGetName( conn, &givenname, &familyname, &fullname, &ok );
            (( dapi_AddressBookGetName_callback ) data->callback )( conn, data->seq, givenname, familyname, fullname, ok );
            break;
            }
        case DAPI_REPLY_ADDRESSBOOKGETEMAILS:
            {
            stringarr emaillist;
            int ok;
            dapi_readReplyAddressBookGetEmails( conn, &emaillist, &ok );
            (( dapi_AddressBookGetEmails_callback ) data->callback )( conn, data->seq, emaillist, ok );
            break;
            }
        case DAPI_REPLY_ADDRESSBOOKFINDBYNAME:
            {
            stringarr idlist;
            int ok;
            dapi_readReplyAddressBookFindByName( conn, &idlist, &ok );
            (( dapi_AddressBookFindByName_callback ) data->callback )( conn, data->seq, idlist, ok );
            break;
            }
        case DAPI_REPLY_ADDRESSBOOKOWNER:
            {
            char* id;
            int ok;
            dapi_readReplyAddressBookOwner( conn, &id, &ok );
            (( dapi_AddressBookOwner_callback ) data->callback )( conn, data->seq, id, ok );
            break;
            }
        case DAPI_REPLY_ADDRESSBOOKGETVCARD30:
            {
            char* vcard;
            int ok;
            dapi_readReplyAddressBookGetVCard30( conn, &vcard, &ok );
            (( dapi_AddressBookGetVCard30_callback ) data->callback )( conn, data->seq, vcard, ok );
            break;
            }
        }
    }
int dapi_callbackOpenUrl_Window( DapiConnection* conn, const char* url, long winfo,
    dapi_OpenUrl_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackOpenUrl( conn, url, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_callbackExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo,
    dapi_ExecuteUrl_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackExecuteUrl( conn, url, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_callbackRunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo, dapi_RunAsUser_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackRunAsUser( conn, user, command, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_callbackMailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo,
    dapi_MailTo_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackMailTo( conn, subject, body, to, cc, bcc, attachments, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_callbackLocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo, dapi_LocalFile_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackLocalFile( conn, remote, local, allow_download, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_callbackUploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo, dapi_UploadFile_callback callback )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_callbackUploadFile( conn, local, file, remove_local, winfo_, callback );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

