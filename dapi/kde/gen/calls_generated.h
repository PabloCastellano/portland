int dapi_Init( DapiConnection* conn );
int dapi_Capabilities( DapiConnection* conn, intarr* capabitilies );
int dapi_OpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo );
int dapi_OpenUrl_Window( DapiConnection* conn, const char* url, long winfo );
int dapi_ExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo );
int dapi_ExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo );
int dapi_ButtonOrder( DapiConnection* conn );
int dapi_RunAsUser( DapiConnection* conn, const char* user, const char* command, DapiWindowInfo winfo );
int dapi_RunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo );
int dapi_SuspendScreensaving( DapiConnection* conn, int suspend );
int dapi_MailTo( DapiConnection* conn, const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo );
int dapi_MailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo );
char* dapi_LocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, DapiWindowInfo winfo );
char* dapi_LocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo );
int dapi_UploadFile( DapiConnection* conn, const char* local, const char* file, int remove_local,
    DapiWindowInfo winfo );
int dapi_UploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo );
int dapi_RemoveTemporaryLocalFile( DapiConnection* conn, const char* local );
