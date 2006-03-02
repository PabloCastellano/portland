int dapi_Init( DapiConnection* conn );
int dapi_OpenUrl( DapiConnection* conn, const char* url );
int dapi_ExecuteUrl( DapiConnection* conn, const char* url );
int dapi_ButtonOrder( DapiConnection* conn );
int dapi_RunAsUser( DapiConnection* conn, const char* user, const char* command );
int dapi_SuspendScreensaving( DapiConnection* conn, int suspend );
int dapi_MailTo( DapiConnection* conn, const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, const char** attachments );
char* dapi_LocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download );
int dapi_UploadFile( DapiConnection* conn, const char* local, const char* file, int remove_local );
int dapi_RemoveTemporaryLocalFile( DapiConnection* conn, const char* local );
