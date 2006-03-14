typedef void( * dapi_Init_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackInit( DapiConnection* conn, dapi_Init_callback callback );
typedef void( * dapi_Capabilities_callback )( DapiConnection* conn, int seq, intarr capabitilies,
    int ok );
int dapi_callbackCapabilities( DapiConnection* conn, dapi_Capabilities_callback callback );
typedef void( * dapi_OpenUrl_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackOpenUrl( DapiConnection* conn, const char* url, dapi_OpenUrl_callback callback );
typedef void( * dapi_ExecuteUrl_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackExecuteUrl( DapiConnection* conn, const char* url, dapi_ExecuteUrl_callback callback );
typedef void( * dapi_ButtonOrder_callback )( DapiConnection* conn, int seq, int order );
int dapi_callbackButtonOrder( DapiConnection* conn, dapi_ButtonOrder_callback callback );
typedef void( * dapi_RunAsUser_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackRunAsUser( DapiConnection* conn, const char* user, const char* command,
    dapi_RunAsUser_callback callback );
typedef void( * dapi_SuspendScreensaving_callback )( DapiConnection* conn, int seq,
    int ok );
int dapi_callbackSuspendScreensaving( DapiConnection* conn, int suspend, dapi_SuspendScreensaving_callback callback );
typedef void( * dapi_MailTo_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, dapi_MailTo_callback callback );
typedef void( * dapi_LocalFile_callback )( DapiConnection* conn, int seq, const char* result );
int dapi_callbackLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, dapi_LocalFile_callback callback );
typedef void( * dapi_UploadFile_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local, dapi_UploadFile_callback callback );
typedef void( * dapi_RemoveTemporaryLocalFile_callback )( DapiConnection* conn, int seq,
    int ok );
int dapi_callbackRemoveTemporaryLocalFile( DapiConnection* conn, const char* local,
    dapi_RemoveTemporaryLocalFile_callback callback );
