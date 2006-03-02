int dapi_readCommand( DapiConnection* conn, int* comm, int* seq )
    {
    command_header command;
    if( readSocket( conn, ( char* ) &command, sizeof( command )) <= 0 )
        return 0;
    if( command.magic != MAGIC )
        return 0;
    *comm = command.command;
    *seq = command.seq;
    return 1;
    }

int dapi_readCommandInit( DapiConnection* conn )
    {
    command_init command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    return 1;
    }

int dapi_readCommandOpenUrl( DapiConnection* conn, char** url )
    {
    command_openurl command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *url = readString( conn, command_.url_len );
    if( *url == NULL )
        {
        free( *url );
        return 0;
        }
    return 1;
    }

int dapi_readCommandExecuteUrl( DapiConnection* conn, char** url )
    {
    command_executeurl command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *url = readString( conn, command_.url_len );
    if( *url == NULL )
        {
        free( *url );
        return 0;
        }
    return 1;
    }

int dapi_readCommandButtonOrder( DapiConnection* conn )
    {
    command_buttonorder command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    return 1;
    }

int dapi_readCommandRunAsUser( DapiConnection* conn, char** user, char** command )
    {
    command_runasuser command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *user = readString( conn, command_.user_len );
    *command = readString( conn, command_.command_len );
    if( *user == NULL || *command == NULL )
        {
        free( *user );
        free( *command );
        return 0;
        }
    return 1;
    }

int dapi_readCommandSuspendScreensaving( DapiConnection* conn, int* suspend )
    {
    command_suspendscreensaving command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *suspend = command_.suspend;
    return 1;
    }

int dapi_readCommandMailTo( DapiConnection* conn, char** subject, char** body, char** to,
    char** cc, char** bcc, char*** attachments )
    {
    command_mailto command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *subject = readString( conn, command_.subject_len );
    *body = readString( conn, command_.body_len );
    *to = readString( conn, command_.to_len );
    *cc = readString( conn, command_.cc_len );
    *bcc = readString( conn, command_.bcc_len );
    *attachments = readStringList( conn, command_.attachments_count );
    if( *subject == NULL || *body == NULL || *to == NULL || *cc == NULL || *bcc == NULL || *attachments == NULL )
        {
        free( *subject );
        free( *body );
        free( *to );
        free( *cc );
        free( *bcc );
        free( *attachments );
        return 0;
        }
    return 1;
    }

int dapi_readCommandLocalFile( DapiConnection* conn, char** remote, char** local,
    int* allow_download )
    {
    command_localfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *allow_download = command_.allow_download;
    *remote = readString( conn, command_.remote_len );
    *local = readString( conn, command_.local_len );
    if( *remote == NULL || *local == NULL )
        {
        free( *remote );
        free( *local );
        return 0;
        }
    return 1;
    }

int dapi_readCommandUploadFile( DapiConnection* conn, char** local, char** file, int* remove_local )
    {
    command_uploadfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *remove_local = command_.remove_local;
    *local = readString( conn, command_.local_len );
    *file = readString( conn, command_.file_len );
    if( *local == NULL || *file == NULL )
        {
        free( *local );
        free( *file );
        return 0;
        }
    return 1;
    }

int dapi_readCommandRemoveTemporaryLocalFile( DapiConnection* conn, char** local )
    {
    command_removetemporarylocalfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *local = readString( conn, command_.local_len );
    if( *local == NULL )
        {
        free( *local );
        return 0;
        }
    return 1;
    }

int dapi_readReplyInit( DapiConnection* conn, int* ok )
    {
    reply_init command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyOpenUrl( DapiConnection* conn, int* ok )
    {
    reply_openurl command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyExecuteUrl( DapiConnection* conn, int* ok )
    {
    reply_executeurl command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyButtonOrder( DapiConnection* conn, int* order )
    {
    reply_buttonorder command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *order = command_.order;
    return 1;
    }

int dapi_readReplyRunAsUser( DapiConnection* conn, int* ok )
    {
    reply_runasuser command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplySuspendScreensaving( DapiConnection* conn, int* ok )
    {
    reply_suspendscreensaving command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyMailTo( DapiConnection* conn, int* ok )
    {
    reply_mailto command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyLocalFile( DapiConnection* conn, char** result )
    {
    reply_localfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *result = readString( conn, command_.result_len );
    if( *result == NULL )
        {
        free( *result );
        return 0;
        }
    return 1;
    }

int dapi_readReplyUploadFile( DapiConnection* conn, int* ok )
    {
    reply_uploadfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

int dapi_readReplyRemoveTemporaryLocalFile( DapiConnection* conn, int* ok )
    {
    reply_removetemporarylocalfile command_;
    if( readSocket( conn, ( char* ) &command_, sizeof( command_ )) <= 0 )
        return 0;
    *ok = command_.ok;
    return 1;
    }

static void writeCommand( DapiConnection* conn, int com, int seq )
    {
    command_header command;
    command.magic = MAGIC;
    command.seq = seq;
    command.command = com;
    writeSocket( conn, ( char* ) &command, sizeof( command ));
    }

int dapi_writeCommandInit( DapiConnection* conn )
    {
    command_init command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_INIT, seq );
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    return seq;
    }

int dapi_writeCommandOpenUrl( DapiConnection* conn, const char* url )
    {
    command_openurl command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_OPENURL, seq );
    command_.url_len = url != NULL ? strlen( url ) : 0;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, url );
    return seq;
    }

int dapi_writeCommandExecuteUrl( DapiConnection* conn, const char* url )
    {
    command_executeurl command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_EXECUTEURL, seq );
    command_.url_len = url != NULL ? strlen( url ) : 0;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, url );
    return seq;
    }

int dapi_writeCommandButtonOrder( DapiConnection* conn )
    {
    command_buttonorder command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_BUTTONORDER, seq );
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    return seq;
    }

int dapi_writeCommandRunAsUser( DapiConnection* conn, const char* user, const char* command )
    {
    command_runasuser command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_RUNASUSER, seq );
    command_.user_len = user != NULL ? strlen( user ) : 0;
    command_.command_len = command != NULL ? strlen( command ) : 0;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, user );
    writeString( conn, command );
    return seq;
    }

int dapi_writeCommandSuspendScreensaving( DapiConnection* conn, int suspend )
    {
    command_suspendscreensaving command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_SUSPENDSCREENSAVING, seq );
    command_.suspend = suspend;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    return seq;
    }

int dapi_writeCommandMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, const char** attachments )
    {
    command_mailto command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_MAILTO, seq );
    command_.subject_len = subject != NULL ? strlen( subject ) : 0;
    command_.body_len = body != NULL ? strlen( body ) : 0;
    command_.to_len = to != NULL ? strlen( to ) : 0;
    command_.cc_len = cc != NULL ? strlen( cc ) : 0;
    command_.bcc_len = bcc != NULL ? strlen( bcc ) : 0;
    command_.attachments_count = stringSize( attachments );
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, subject );
    writeString( conn, body );
    writeString( conn, to );
    writeString( conn, cc );
    writeString( conn, bcc );
    writeStringList( conn, attachments );
    return seq;
    }

int dapi_writeCommandLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download )
    {
    command_localfile command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_LOCALFILE, seq );
    command_.remote_len = remote != NULL ? strlen( remote ) : 0;
    command_.local_len = local != NULL ? strlen( local ) : 0;
    command_.allow_download = allow_download;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, remote );
    writeString( conn, local );
    return seq;
    }

int dapi_writeCommandUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local )
    {
    command_uploadfile command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_UPLOADFILE, seq );
    command_.local_len = local != NULL ? strlen( local ) : 0;
    command_.file_len = file != NULL ? strlen( file ) : 0;
    command_.remove_local = remove_local;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, local );
    writeString( conn, file );
    return seq;
    }

int dapi_writeCommandRemoveTemporaryLocalFile( DapiConnection* conn, const char* local )
    {
    command_removetemporarylocalfile command_;
    int seq = getNextSeq( conn );
    writeCommand( conn, DAPI_COMMAND_REMOVETEMPORARYLOCALFILE, seq );
    command_.local_len = local != NULL ? strlen( local ) : 0;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, local );
    return seq;
    }

void dapi_writeReplyInit( DapiConnection* conn, int seq, int ok )
    {
    reply_init command_;
    writeCommand( conn, DAPI_REPLY_INIT, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyOpenUrl( DapiConnection* conn, int seq, int ok )
    {
    reply_openurl command_;
    writeCommand( conn, DAPI_REPLY_OPENURL, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyExecuteUrl( DapiConnection* conn, int seq, int ok )
    {
    reply_executeurl command_;
    writeCommand( conn, DAPI_REPLY_EXECUTEURL, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyButtonOrder( DapiConnection* conn, int seq, int order )
    {
    reply_buttonorder command_;
    writeCommand( conn, DAPI_REPLY_BUTTONORDER, seq );
    command_.order = order;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyRunAsUser( DapiConnection* conn, int seq, int ok )
    {
    reply_runasuser command_;
    writeCommand( conn, DAPI_REPLY_RUNASUSER, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplySuspendScreensaving( DapiConnection* conn, int seq, int ok )
    {
    reply_suspendscreensaving command_;
    writeCommand( conn, DAPI_REPLY_SUSPENDSCREENSAVING, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyMailTo( DapiConnection* conn, int seq, int ok )
    {
    reply_mailto command_;
    writeCommand( conn, DAPI_REPLY_MAILTO, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyLocalFile( DapiConnection* conn, int seq, const char* result )
    {
    reply_localfile command_;
    writeCommand( conn, DAPI_REPLY_LOCALFILE, seq );
    command_.result_len = result != NULL ? strlen( result ) : 0;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    writeString( conn, result );
    }

void dapi_writeReplyUploadFile( DapiConnection* conn, int seq, int ok )
    {
    reply_uploadfile command_;
    writeCommand( conn, DAPI_REPLY_UPLOADFILE, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

void dapi_writeReplyRemoveTemporaryLocalFile( DapiConnection* conn, int seq, int ok )
    {
    reply_removetemporarylocalfile command_;
    writeCommand( conn, DAPI_REPLY_REMOVETEMPORARYLOCALFILE, seq );
    command_.ok = ok;
    writeSocket( conn, ( char* ) &command_, sizeof( command_ ));
    }

