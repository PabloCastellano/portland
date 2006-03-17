int dapi_readCommandInit( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandCapabilities( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandOpenUrl( DapiConnection* conn, char** url, DapiWindowInfo* winfo )
    {
    *url = readString( conn );
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandExecuteUrl( DapiConnection* conn, char** url, DapiWindowInfo* winfo )
    {
    *url = readString( conn );
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandButtonOrder( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandRunAsUser( DapiConnection* conn, char** user, char** command,
    DapiWindowInfo* winfo )
    {
    *user = readString( conn );
    *command = readString( conn );
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandSuspendScreensaving( DapiConnection* conn, int* suspend )
    {
    readSocket( conn, suspend, sizeof( *suspend ));
    return 1;
    }

int dapi_readCommandMailTo( DapiConnection* conn, char** subject, char** body, char** to,
    char** cc, char** bcc, stringarr* attachments, DapiWindowInfo* winfo )
    {
    *subject = readString( conn );
    *body = readString( conn );
    *to = readString( conn );
    *cc = readString( conn );
    *bcc = readString( conn );
    *attachments = readstringarr( conn );
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandLocalFile( DapiConnection* conn, char** remote, char** local,
    int* allow_download, DapiWindowInfo* winfo )
    {
    *remote = readString( conn );
    *local = readString( conn );
    readSocket( conn, allow_download, sizeof( *allow_download ));
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandUploadFile( DapiConnection* conn, char** local, char** file, int* remove_local,
    DapiWindowInfo* winfo )
    {
    *local = readString( conn );
    *file = readString( conn );
    readSocket( conn, remove_local, sizeof( *remove_local ));
    *winfo = readWindowInfo( conn );
    return 1;
    }

int dapi_readCommandRemoveTemporaryLocalFile( DapiConnection* conn, char** local )
    {
    *local = readString( conn );
    return 1;
    }

int dapi_readCommandAddressBookList( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandAddressBookGetName( DapiConnection* conn, char** id )
    {
    *id = readString( conn );
    return 1;
    }

int dapi_readCommandAddressBookGetEmails( DapiConnection* conn, char** id )
    {
    *id = readString( conn );
    return 1;
    }

int dapi_readCommandAddressBookFindByName( DapiConnection* conn, char** name )
    {
    *name = readString( conn );
    return 1;
    }

int dapi_readCommandAddressBookOwner( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandAddressBookGetVCard30( DapiConnection* conn, char** id )
    {
    *id = readString( conn );
    return 1;
    }

int dapi_readReplyInit( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyCapabilities( DapiConnection* conn, intarr* capabitilies, int* ok )
    {
    *capabitilies = readintarr( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyOpenUrl( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyExecuteUrl( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyButtonOrder( DapiConnection* conn, int* order )
    {
    readSocket( conn, order, sizeof( *order ));
    return 1;
    }

int dapi_readReplyRunAsUser( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplySuspendScreensaving( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyMailTo( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyLocalFile( DapiConnection* conn, char** result )
    {
    *result = readString( conn );
    return 1;
    }

int dapi_readReplyUploadFile( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyRemoveTemporaryLocalFile( DapiConnection* conn, int* ok )
    {
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookList( DapiConnection* conn, stringarr* idlist, int* ok )
    {
    *idlist = readstringarr( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookGetName( DapiConnection* conn, char** givenname, char** familyname,
    char** fullname, int* ok )
    {
    *givenname = readString( conn );
    *familyname = readString( conn );
    *fullname = readString( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookGetEmails( DapiConnection* conn, stringarr* emaillist,
    int* ok )
    {
    *emaillist = readstringarr( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookFindByName( DapiConnection* conn, stringarr* idlist,
    int* ok )
    {
    *idlist = readstringarr( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookOwner( DapiConnection* conn, char** id, int* ok )
    {
    *id = readString( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_readReplyAddressBookGetVCard30( DapiConnection* conn, char** vcard, int* ok )
    {
    *vcard = readString( conn );
    readSocket( conn, ok, sizeof( *ok ));
    return 1;
    }

int dapi_writeCommandInit( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_INIT, seq );
    return seq;
    }

int dapi_writeCommandCapabilities( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_CAPABILITIES, seq );
    return seq;
    }

int dapi_writeCommandOpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_OPENURL, seq );
    writeString( conn, url );
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_EXECUTEURL, seq );
    writeString( conn, url );
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandButtonOrder( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_BUTTONORDER, seq );
    return seq;
    }

int dapi_writeCommandRunAsUser( DapiConnection* conn, const char* user, const char* command,
    DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_RUNASUSER, seq );
    writeString( conn, user );
    writeString( conn, command );
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandSuspendScreensaving( DapiConnection* conn, int suspend )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_SUSPENDSCREENSAVING, seq );
    writeSocket( conn, &suspend, sizeof( suspend ));
    return seq;
    }

int dapi_writeCommandMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_MAILTO, seq );
    writeString( conn, subject );
    writeString( conn, body );
    writeString( conn, to );
    writeString( conn, cc );
    writeString( conn, bcc );
    writestringarr( conn, attachments );
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_LOCALFILE, seq );
    writeString( conn, remote );
    writeString( conn, local );
    writeSocket( conn, &allow_download, sizeof( allow_download ));
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local, DapiWindowInfo winfo )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_UPLOADFILE, seq );
    writeString( conn, local );
    writeString( conn, file );
    writeSocket( conn, &remove_local, sizeof( remove_local ));
    writeWindowInfo( conn, winfo );
    return seq;
    }

int dapi_writeCommandRemoveTemporaryLocalFile( DapiConnection* conn, const char* local )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_REMOVETEMPORARYLOCALFILE, seq );
    writeString( conn, local );
    return seq;
    }

int dapi_writeCommandAddressBookList( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKLIST, seq );
    return seq;
    }

int dapi_writeCommandAddressBookGetName( DapiConnection* conn, const char* id )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKGETNAME, seq );
    writeString( conn, id );
    return seq;
    }

int dapi_writeCommandAddressBookGetEmails( DapiConnection* conn, const char* id )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKGETEMAILS, seq );
    writeString( conn, id );
    return seq;
    }

int dapi_writeCommandAddressBookFindByName( DapiConnection* conn, const char* name )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKFINDBYNAME, seq );
    writeString( conn, name );
    return seq;
    }

int dapi_writeCommandAddressBookOwner( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKOWNER, seq );
    return seq;
    }

int dapi_writeCommandAddressBookGetVCard30( DapiConnection* conn, const char* id )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_ADDRESSBOOKGETVCARD30, seq );
    writeString( conn, id );
    return seq;
    }

void dapi_writeReplyInit( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_INIT, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyCapabilities( DapiConnection* conn, int seq, intarr capabitilies,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_CAPABILITIES, seq );
    writeintarr( conn, capabitilies );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyOpenUrl( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_OPENURL, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyExecuteUrl( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_EXECUTEURL, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyButtonOrder( DapiConnection* conn, int seq, int order )
    {
    writeCommand( conn, DAPI_REPLY_BUTTONORDER, seq );
    writeSocket( conn, &order, sizeof( order ));
    }

void dapi_writeReplyRunAsUser( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_RUNASUSER, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplySuspendScreensaving( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_SUSPENDSCREENSAVING, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyMailTo( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_MAILTO, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyLocalFile( DapiConnection* conn, int seq, const char* result )
    {
    writeCommand( conn, DAPI_REPLY_LOCALFILE, seq );
    writeString( conn, result );
    }

void dapi_writeReplyUploadFile( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_UPLOADFILE, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyRemoveTemporaryLocalFile( DapiConnection* conn, int seq, int ok )
    {
    writeCommand( conn, DAPI_REPLY_REMOVETEMPORARYLOCALFILE, seq );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookList( DapiConnection* conn, int seq, stringarr idlist,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKLIST, seq );
    writestringarr( conn, idlist );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookGetName( DapiConnection* conn, int seq, const char* givenname,
    const char* familyname, const char* fullname, int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKGETNAME, seq );
    writeString( conn, givenname );
    writeString( conn, familyname );
    writeString( conn, fullname );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookGetEmails( DapiConnection* conn, int seq, stringarr emaillist,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKGETEMAILS, seq );
    writestringarr( conn, emaillist );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookFindByName( DapiConnection* conn, int seq, stringarr idlist,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKFINDBYNAME, seq );
    writestringarr( conn, idlist );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookOwner( DapiConnection* conn, int seq, const char* id,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKOWNER, seq );
    writeString( conn, id );
    writeSocket( conn, &ok, sizeof( ok ));
    }

void dapi_writeReplyAddressBookGetVCard30( DapiConnection* conn, int seq, const char* vcard,
    int ok )
    {
    writeCommand( conn, DAPI_REPLY_ADDRESSBOOKGETVCARD30, seq );
    writeString( conn, vcard );
    writeSocket( conn, &ok, sizeof( ok ));
    }

int dapi_writeCommandOpenUrl_Window( DapiConnection* conn, const char* url, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandOpenUrl( conn, url, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_writeCommandExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandExecuteUrl( conn, url, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_writeCommandRunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandRunAsUser( conn, user, command, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_writeCommandMailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandMailTo( conn, subject, body, to, cc, bcc, attachments, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_writeCommandLocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandLocalFile( conn, remote, local, allow_download, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

int dapi_writeCommandUploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo )
    {
    DapiWindowInfo winfo_;
    dapi_windowInfoInitWindow( &winfo_, winfo );
    int seq = dapi_writeCommandUploadFile( conn, local, file, remove_local, winfo_ );
    dapi_freeWindowInfo( winfo_ );
    return seq;
    }

