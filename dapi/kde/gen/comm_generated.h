int dapi_readCommand( DapiConnection* conn, int* comm, int* seq );
int dapi_readCommandInit( DapiConnection* conn );
int dapi_writeCommandInit( DapiConnection* conn );
int dapi_readReplyInit( DapiConnection* conn, int* ok );
void dapi_writeReplyInit( DapiConnection* conn, int seq, int ok );
int dapi_readCommandCapabilities( DapiConnection* conn );
int dapi_writeCommandCapabilities( DapiConnection* conn );
int dapi_readReplyCapabilities( DapiConnection* conn, intarr* capabitilies, int* ok );
void dapi_writeReplyCapabilities( DapiConnection* conn, int seq, intarr capabitilies,
    int ok );
int dapi_readCommandOpenUrl( DapiConnection* conn, char** url, DapiWindowInfo* winfo );
int dapi_writeCommandOpenUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo );
int dapi_writeCommandOpenUrl_Window( DapiConnection* conn, const char* url, long winfo );
int dapi_readReplyOpenUrl( DapiConnection* conn, int* ok );
void dapi_writeReplyOpenUrl( DapiConnection* conn, int seq, int ok );
int dapi_readCommandExecuteUrl( DapiConnection* conn, char** url, DapiWindowInfo* winfo );
int dapi_writeCommandExecuteUrl( DapiConnection* conn, const char* url, DapiWindowInfo winfo );
int dapi_writeCommandExecuteUrl_Window( DapiConnection* conn, const char* url, long winfo );
int dapi_readReplyExecuteUrl( DapiConnection* conn, int* ok );
void dapi_writeReplyExecuteUrl( DapiConnection* conn, int seq, int ok );
int dapi_readCommandButtonOrder( DapiConnection* conn );
int dapi_writeCommandButtonOrder( DapiConnection* conn );
int dapi_readReplyButtonOrder( DapiConnection* conn, int* order );
void dapi_writeReplyButtonOrder( DapiConnection* conn, int seq, int order );
int dapi_readCommandRunAsUser( DapiConnection* conn, char** user, char** command,
    DapiWindowInfo* winfo );
int dapi_writeCommandRunAsUser( DapiConnection* conn, const char* user, const char* command,
    DapiWindowInfo winfo );
int dapi_writeCommandRunAsUser_Window( DapiConnection* conn, const char* user, const char* command,
    long winfo );
int dapi_readReplyRunAsUser( DapiConnection* conn, int* ok );
void dapi_writeReplyRunAsUser( DapiConnection* conn, int seq, int ok );
int dapi_readCommandSuspendScreensaving( DapiConnection* conn, int* suspend );
int dapi_writeCommandSuspendScreensaving( DapiConnection* conn, int suspend );
int dapi_readReplySuspendScreensaving( DapiConnection* conn, int* ok );
void dapi_writeReplySuspendScreensaving( DapiConnection* conn, int seq, int ok );
int dapi_readCommandMailTo( DapiConnection* conn, char** subject, char** body, char** to,
    char** cc, char** bcc, stringarr* attachments, DapiWindowInfo* winfo );
int dapi_writeCommandMailTo( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, DapiWindowInfo winfo );
int dapi_writeCommandMailTo_Window( DapiConnection* conn, const char* subject, const char* body,
    const char* to, const char* cc, const char* bcc, stringarr attachments, long winfo );
int dapi_readReplyMailTo( DapiConnection* conn, int* ok );
void dapi_writeReplyMailTo( DapiConnection* conn, int seq, int ok );
int dapi_readCommandLocalFile( DapiConnection* conn, char** remote, char** local,
    int* allow_download, DapiWindowInfo* winfo );
int dapi_writeCommandLocalFile( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, DapiWindowInfo winfo );
int dapi_writeCommandLocalFile_Window( DapiConnection* conn, const char* remote, const char* local,
    int allow_download, long winfo );
int dapi_readReplyLocalFile( DapiConnection* conn, char** result );
void dapi_writeReplyLocalFile( DapiConnection* conn, int seq, const char* result );
int dapi_readCommandUploadFile( DapiConnection* conn, char** local, char** file, int* remove_local,
    DapiWindowInfo* winfo );
int dapi_writeCommandUploadFile( DapiConnection* conn, const char* local, const char* file,
    int remove_local, DapiWindowInfo winfo );
int dapi_writeCommandUploadFile_Window( DapiConnection* conn, const char* local, const char* file,
    int remove_local, long winfo );
int dapi_readReplyUploadFile( DapiConnection* conn, int* ok );
void dapi_writeReplyUploadFile( DapiConnection* conn, int seq, int ok );
int dapi_readCommandRemoveTemporaryLocalFile( DapiConnection* conn, char** local );
int dapi_writeCommandRemoveTemporaryLocalFile( DapiConnection* conn, const char* local );
int dapi_readReplyRemoveTemporaryLocalFile( DapiConnection* conn, int* ok );
void dapi_writeReplyRemoveTemporaryLocalFile( DapiConnection* conn, int seq, int ok );
int dapi_readCommandAddressBookList( DapiConnection* conn );
int dapi_writeCommandAddressBookList( DapiConnection* conn );
int dapi_readReplyAddressBookList( DapiConnection* conn, stringarr* idlist, int* ok );
void dapi_writeReplyAddressBookList( DapiConnection* conn, int seq, stringarr idlist,
    int ok );
int dapi_readCommandAddressBookGetName( DapiConnection* conn, char** id );
int dapi_writeCommandAddressBookGetName( DapiConnection* conn, const char* id );
int dapi_readReplyAddressBookGetName( DapiConnection* conn, char** givenname, char** familyname,
    char** fullname, int* ok );
void dapi_writeReplyAddressBookGetName( DapiConnection* conn, int seq, const char* givenname,
    const char* familyname, const char* fullname, int ok );
int dapi_readCommandAddressBookGetEmails( DapiConnection* conn, char** id );
int dapi_writeCommandAddressBookGetEmails( DapiConnection* conn, const char* id );
int dapi_readReplyAddressBookGetEmails( DapiConnection* conn, stringarr* emaillist,
    int* ok );
void dapi_writeReplyAddressBookGetEmails( DapiConnection* conn, int seq, stringarr emaillist,
    int ok );
int dapi_readCommandAddressBookFindByName( DapiConnection* conn, char** name );
int dapi_writeCommandAddressBookFindByName( DapiConnection* conn, const char* name );
int dapi_readReplyAddressBookFindByName( DapiConnection* conn, stringarr* idlist,
    int* ok );
void dapi_writeReplyAddressBookFindByName( DapiConnection* conn, int seq, stringarr idlist,
    int ok );
int dapi_readCommandAddressBookOwner( DapiConnection* conn );
int dapi_writeCommandAddressBookOwner( DapiConnection* conn );
int dapi_readReplyAddressBookOwner( DapiConnection* conn, char** id, int* ok );
void dapi_writeReplyAddressBookOwner( DapiConnection* conn, int seq, const char* id,
    int ok );
int dapi_readCommandAddressBookGetVCard30( DapiConnection* conn, char** id );
int dapi_writeCommandAddressBookGetVCard30( DapiConnection* conn, const char* id );
int dapi_readReplyAddressBookGetVCard30( DapiConnection* conn, char** vcard, int* ok );
void dapi_writeReplyAddressBookGetVCard30( DapiConnection* conn, int seq, const char* vcard,
    int ok );
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
    DAPI_REPLY_REMOVETEMPORARYLOCALFILE,
    DAPI_COMMAND_ADDRESSBOOKLIST,
    DAPI_REPLY_ADDRESSBOOKLIST,
    DAPI_COMMAND_ADDRESSBOOKGETNAME,
    DAPI_REPLY_ADDRESSBOOKGETNAME,
    DAPI_COMMAND_ADDRESSBOOKGETEMAILS,
    DAPI_REPLY_ADDRESSBOOKGETEMAILS,
    DAPI_COMMAND_ADDRESSBOOKFINDBYNAME,
    DAPI_REPLY_ADDRESSBOOKFINDBYNAME,
    DAPI_COMMAND_ADDRESSBOOKOWNER,
    DAPI_REPLY_ADDRESSBOOKOWNER,
    DAPI_COMMAND_ADDRESSBOOKGETVCARD30,
    DAPI_REPLY_ADDRESSBOOKGETVCARD30
    };
