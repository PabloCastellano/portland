int dapi_readCommand( DapiConnection* conn, int* comm, int* seq );
int dapi_readCommandInit( DapiConnection* conn );
int dapi_writeCommandInit( DapiConnection* conn );
int dapi_readReplyInit( DapiConnection* conn, int* ok );
void dapi_writeReplyInit( DapiConnection* conn, int seq, int ok );
int dapi_readCommandcapabilities( DapiConnection* conn );
int dapi_writeCommandcapabilities( DapiConnection* conn );
int dapi_readReplycapabilities( DapiConnection* conn, intarr* capabitilies, int* ok );
void dapi_writeReplycapabilities( DapiConnection* conn, int seq, intarr capabitilies,
    int ok );
int dapi_readCommandOpenUrl( DapiConnection* conn, char** url );
int dapi_writeCommandOpenUrl( DapiConnection* conn, const char* url );
int dapi_readReplyOpenUrl( DapiConnection* conn, int* ok );
void dapi_writeReplyOpenUrl( DapiConnection* conn, int seq, int ok );
int dapi_readCommandExecuteUrl( DapiConnection* conn, char** url );
int dapi_writeCommandExecuteUrl( DapiConnection* conn, const char* url );
int dapi_readReplyExecuteUrl( DapiConnection* conn, int* ok );
void dapi_writeReplyExecuteUrl( DapiConnection* conn, int seq, int ok );
int dapi_readCommandButtonOrder( DapiConnection* conn );
int dapi_writeCommandButtonOrder( DapiConnection* conn );
int dapi_readReplyButtonOrder( DapiConnection* conn, int* order );
void dapi_writeReplyButtonOrder( DapiConnection* conn, int seq, int order );
int dapi_readCommandRunAsUser( DapiConnection* conn, char** user, char** command );
int dapi_writeCommandRunAsUser( DapiConnection* conn, const char* user, const char* command );
int dapi_readReplyRunAsUser( DapiConnection* conn, int* ok );
void dapi_writeReplyRunAsUser( DapiConnection* conn, int seq, int ok );
int dapi_readCommandSuspendScreensaving( DapiConnection* conn, int* suspend );
int dapi_writeCommandSuspendScreensaving( DapiConnection* conn, int suspend );
int dapi_readReplySuspendScreensaving( DapiConnection* conn, int* ok );
void dapi_writeReplySuspendScreensaving( DapiConnection* conn, int seq, int ok );
int dapi_readCommandMailTo( DapiConnection* conn, char** subject, char** body, char** to,
    char** cc, char** bcc, stringarr* attachments );
int dapi_writeCommandMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments );
int dapi_readReplyMailTo( DapiConnection* conn, int* ok );
void dapi_writeReplyMailTo( DapiConnection* conn, int seq, int ok );
int dapi_readCommandLocalFile( DapiConnection* conn, char** remote, char** local,
    int* allow_download );
int dapi_writeCommandLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download );
int dapi_readReplyLocalFile( DapiConnection* conn, char** result );
void dapi_writeReplyLocalFile( DapiConnection* conn, int seq, const char* result );
int dapi_readCommandUploadFile( DapiConnection* conn, char** local, char** file, int* remove_local );
int dapi_writeCommandUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local );
int dapi_readReplyUploadFile( DapiConnection* conn, int* ok );
void dapi_writeReplyUploadFile( DapiConnection* conn, int seq, int ok );
int dapi_readCommandRemoveTemporaryLocalFile( DapiConnection* conn, char** local );
int dapi_writeCommandRemoveTemporaryLocalFile( DapiConnection* conn, const char* local );
int dapi_readReplyRemoveTemporaryLocalFile( DapiConnection* conn, int* ok );
void dapi_writeReplyRemoveTemporaryLocalFile( DapiConnection* conn, int seq, int ok );
enum
    {
    DAPI_COMMAND_INIT,
    DAPI_REPLY_INIT,
    DAPI_COMMAND_CAPABILITIES,
    DAPI_REPLY_CAPABILITIES,
    DAPI_COMMAND_OPENURL,
    DAPI_REPLY_OPENURL,
    DAPI_COMMAND_EXECUTEURL,
    DAPI_REPLY_EXECUTEURL,
    DAPI_COMMAND_BUTTONORDER,
    DAPI_REPLY_BUTTONORDER,
    DAPI_COMMAND_RUNASUSER,
    DAPI_REPLY_RUNASUSER,
    DAPI_COMMAND_SUSPENDSCREENSAVING,
    DAPI_REPLY_SUSPENDSCREENSAVING,
    DAPI_COMMAND_MAILTO,
    DAPI_REPLY_MAILTO,
    DAPI_COMMAND_LOCALFILE,
    DAPI_REPLY_LOCALFILE,
    DAPI_COMMAND_UPLOADFILE,
    DAPI_REPLY_UPLOADFILE,
    DAPI_COMMAND_REMOVETEMPORARYLOCALFILE,
    DAPI_REPLY_REMOVETEMPORARYLOCALFILE
    };
