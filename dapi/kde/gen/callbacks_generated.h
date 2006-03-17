typedef void( * dapi_Init_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackInit( DapiConnection* conn, dapi_Init_callback callback );
typedef void( * dapi_Capabilities_callback )( DapiConnection* conn, int seq, intarr capabitilies,
    int ok );
int dapi_callbackCapabilities( DapiConnection* conn, dapi_Capabilities_callback callback );
typedef void( * dapi_OpenUrl_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackOpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo,
    dapi_OpenUrl_callback callback );
int dapi_callbackOpenUrl_Window( DapiConnection* conn, const char* url, long winfo,
    dapi_OpenUrl_callback callback );
typedef void( * dapi_ExecuteUrl_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo,
    dapi_ExecuteUrl_callback callback );
int dapi_callbackExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo,
    dapi_ExecuteUrl_callback callback );
typedef void( * dapi_ButtonOrder_callback )( DapiConnection* conn, int seq, int order );
int dapi_callbackButtonOrder( DapiConnection* conn, dapi_ButtonOrder_callback callback );
typedef void( * dapi_RunAsUser_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackRunAsUser( DapiConnection* conn, const char* user, const char* command,
    DapiWindowInfo winfo, dapi_RunAsUser_callback callback );
int dapi_callbackRunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo, dapi_RunAsUser_callback callback );
typedef void( * dapi_SuspendScreensaving_callback )( DapiConnection* conn, int seq,
    int ok );
int dapi_callbackSuspendScreensaving( DapiConnection* conn, int suspend, dapi_SuspendScreensaving_callback callback );
typedef void( * dapi_MailTo_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo,
    dapi_MailTo_callback callback );
int dapi_callbackMailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo,
    dapi_MailTo_callback callback );
typedef void( * dapi_LocalFile_callback )( DapiConnection* conn, int seq, const char* result );
int dapi_callbackLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, DapiWindowInfo winfo, dapi_LocalFile_callback callback );
int dapi_callbackLocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo, dapi_LocalFile_callback callback );
typedef void( * dapi_UploadFile_callback )( DapiConnection* conn, int seq, int ok );
int dapi_callbackUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local, DapiWindowInfo winfo, dapi_UploadFile_callback callback );
int dapi_callbackUploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo, dapi_UploadFile_callback callback );
typedef void( * dapi_RemoveTemporaryLocalFile_callback )( DapiConnection* conn, int seq,
    int ok );
int dapi_callbackRemoveTemporaryLocalFile( DapiConnection* conn, const char* local,
    dapi_RemoveTemporaryLocalFile_callback callback );
typedef void( * dapi_AddressBookList_callback )( DapiConnection* conn, int seq, stringarr idlist,
    int ok );
int dapi_callbackAddressBookList( DapiConnection* conn, dapi_AddressBookList_callback callback );
typedef void( * dapi_AddressBookGetName_callback )( DapiConnection* conn, int seq,
    const char* givenname, const char* familyname, const char* fullname, int ok );
int dapi_callbackAddressBookGetName( DapiConnection* conn, const char* id, dapi_AddressBookGetName_callback callback );
typedef void( * dapi_AddressBookGetEmails_callback )( DapiConnection* conn, int seq,
    stringarr emaillist, int ok );
int dapi_callbackAddressBookGetEmails( DapiConnection* conn, const char* id, dapi_AddressBookGetEmails_callback callback );
typedef void( * dapi_AddressBookFindByName_callback )( DapiConnection* conn, int seq,
    stringarr idlist, int ok );
int dapi_callbackAddressBookFindByName( DapiConnection* conn, const char* name, dapi_AddressBookFindByName_callback callback );
typedef void( * dapi_AddressBookOwner_callback )( DapiConnection* conn, int seq, const char* id,
    int ok );
int dapi_callbackAddressBookOwner( DapiConnection* conn, dapi_AddressBookOwner_callback callback );
typedef void( * dapi_AddressBookGetVCard30_callback )( DapiConnection* conn, int seq,
    const char* vcard, int ok );
int dapi_callbackAddressBookGetVCard30( DapiConnection* conn, const char* id, dapi_AddressBookGetVCard30_callback callback );
