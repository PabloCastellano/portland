int dapi_readCommandInit( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandCapabilities( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandOpenUrl( DapiConnection* conn, char** url )
    {
    *url = readString( conn );
    return 1;
    }

int dapi_readCommandExecuteUrl( DapiConnection* conn, char** url )
    {
    *url = readString( conn );
    return 1;
    }

int dapi_readCommandButtonOrder( DapiConnection* conn )
    {
    return 1;
    }

int dapi_readCommandRunAsUser( DapiConnection* conn, char** user, char** command )
    {
    *user = readString( conn );
    *command = readString( conn );
    return 1;
    }

int dapi_readCommandSuspendScreensaving( DapiConnection* conn, int* suspend )
    {
    readSocket( conn, suspend, sizeof( *suspend ));
    return 1;
    }

int dapi_readCommandMailTo( DapiConnection* conn, char** subject, char** body, char** to,
    char** cc, char** bcc, stringarr* attachments )
    {
    *subject = readString( conn );
    *body = readString( conn );
    *to = readString( conn );
    *cc = readString( conn );
    *bcc = readString( conn );
    *attachments = readstringarr( conn );
    return 1;
    }

int dapi_readCommandLocalFile( DapiConnection* conn, char** remote, char** local,
    int* allow_download )
    {
    *remote = readString( conn );
    *local = readString( conn );
    readSocket( conn, allow_download, sizeof( *allow_download ));
    return 1;
    }

int dapi_readCommandUploadFile( DapiConnection* conn, char** local, char** file, int* remove_local )
    {
    *local = readString( conn );
    *file = readString( conn );
    readSocket( conn, remove_local, sizeof( *remove_local ));
    return 1;
    }

int dapi_readCommandRemoveTemporaryLocalFile( DapiConnection* conn, char** local )
    {
    *local = readString( conn );
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

int dapi_writeCommandOpenUrl( DapiConnection* conn, const char* url )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_OPENURL, seq );
    writeString( conn, url );
    return seq;
    }

int dapi_writeCommandExecuteUrl( DapiConnection* conn, const char* url )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_EXECUTEURL, seq );
    writeString( conn, url );
    return seq;
    }

int dapi_writeCommandButtonOrder( DapiConnection* conn )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_BUTTONORDER, seq );
    return seq;
    }

int dapi_writeCommandRunAsUser( DapiConnection* conn, const char* user, const char* command )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_RUNASUSER, seq );
    writeString( conn, user );
    writeString( conn, command );
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
    const char* to, const char* cc, const char* bcc, stringarr attachments )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_MAILTO, seq );
    writeString( conn, subject );
    writeString( conn, body );
    writeString( conn, to );
    writeString( conn, cc );
    writeString( conn, bcc );
    writestringarr( conn, attachments );
    return seq;
    }

int dapi_writeCommandLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_LOCALFILE, seq );
    writeString( conn, remote );
    writeString( conn, local );
    writeSocket( conn, &allow_download, sizeof( allow_download ));
    return seq;
    }

int dapi_writeCommandUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_UPLOADFILE, seq );
    writeString( conn, local );
    writeString( conn, file );
    writeSocket( conn, &remove_local, sizeof( remove_local ));
    return seq;
    }

int dapi_writeCommandRemoveTemporaryLocalFile( DapiConnection* conn, const char* local )
    {
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_REMOVETEMPORARYLOCALFILE, seq );
    writeString( conn, local );
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

