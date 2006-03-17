int dapi_Init( DapiConnection* conn )
    {
    int seq;
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
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyInit( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_Capabilities( DapiConnection* conn, intarr* capabitilies )
    {
    int seq;
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
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyCapabilities( conn, capabitilies, &ret ))
        return 0;
    return ret;
    }

int dapi_OpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandOpenUrl( conn, url, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_OPENURL )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyOpenUrl( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_ExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandExecuteUrl( conn, url, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_EXECUTEURL )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyExecuteUrl( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_ButtonOrder( DapiConnection* conn )
    {
    int seq;
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
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyButtonOrder( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_RunAsUser( DapiConnection* conn, const char* user, const char* command, DapiWindowInfo winfo )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandRunAsUser( conn, user, command, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_RUNASUSER )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyRunAsUser( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_SuspendScreensaving( DapiConnection* conn, int suspend )
    {
    int seq;
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
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplySuspendScreensaving( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_MailTo( DapiConnection* conn, const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandMailTo( conn, subject, body, to, cc, bcc, attachments, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_MAILTO )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyMailTo( conn, &ret ))
        return 0;
    return ret;
    }

char* dapi_LocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, DapiWindowInfo winfo )
    {
    int seq;
    char* ret;
    seq = dapi_writeCommandLocalFile( conn, remote, local, allow_download, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_LOCALFILE )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
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

int dapi_UploadFile( DapiConnection* conn, const char* local, const char* file, int remove_local,
    DapiWindowInfo winfo )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandUploadFile( conn, local, file, remove_local, winfo );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_UPLOADFILE )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyUploadFile( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_RemoveTemporaryLocalFile( DapiConnection* conn, const char* local )
    {
    int seq;
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
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyRemoveTemporaryLocalFile( conn, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookList( DapiConnection* conn, stringarr* idlist )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookList( conn );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKLIST )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookList( conn, idlist, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookGetName( DapiConnection* conn, const char* id, char** givenname,
    char** familyname, char** fullname )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookGetName( conn, id );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKGETNAME )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookGetName( conn, givenname, familyname, fullname, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookGetEmails( DapiConnection* conn, const char* id, stringarr* emaillist )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookGetEmails( conn, id );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKGETEMAILS )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookGetEmails( conn, emaillist, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookFindByName( DapiConnection* conn, const char* name, stringarr* idlist )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookFindByName( conn, name );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKFINDBYNAME )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookFindByName( conn, idlist, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookOwner( DapiConnection* conn, char** id )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookOwner( conn );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKOWNER )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookOwner( conn, id, &ret ))
        return 0;
    return ret;
    }

int dapi_AddressBookGetVCard30( DapiConnection* conn, const char* id, char** vcard )
    {
    int seq;
    int ret;
    seq = dapi_writeCommandAddressBookGetVCard30( conn, id );
    if( seq == 0 )
        return 0;
    for(;;)
        {
        int comm, seq2;
        if( !dapi_readCommand( conn, &comm, &seq2 ))
            return 0;
        if( seq2 == seq && comm == DAPI_REPLY_ADDRESSBOOKGETVCARD30 )
            break; /* --> */
        conn->generic_callback( conn, comm, seq2 );
        }
    if( !dapi_readReplyAddressBookGetVCard30( conn, vcard, &ret ))
        return 0;
    return ret;
    }

int dapi_OpenUrl_Window( DapiConnection* conn, const char* url, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int ret = dapi_OpenUrl( conn, url, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

int dapi_ExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int ret = dapi_ExecuteUrl( conn, url, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

int dapi_RunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int ret = dapi_RunAsUser( conn, user, command, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

int dapi_MailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int ret = dapi_MailTo( conn, subject, body, to, cc, bcc, attachments, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

char* dapi_LocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    char* ret = dapi_LocalFile( conn, remote, local, allow_download, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

int dapi_UploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int ret = dapi_UploadFile( conn, local, file, remove_local, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return ret;
    }

