typedef struct command_header
    {
    int magic;
    int seq;
    int command;
    } command_header;
typedef struct command_init
    {
    int dummy;
    } command_init;
typedef struct command_openurl
    {
    int url_len; /*   char* url   */
    } command_openurl;
typedef struct command_executeurl
    {
    int url_len; /*   char* url   */
    } command_executeurl;
typedef struct command_buttonorder
    {
    int dummy;
    } command_buttonorder;
typedef struct command_runasuser
    {
    int user_len; /*   char* user   */
    int command_len; /*   char* command   */
    } command_runasuser;
typedef struct command_suspendscreensaving
    {
    int suspend;
    } command_suspendscreensaving;
typedef struct command_mailto
    {
    int subject_len; /*   char* subject   */
    int body_len; /*   char* body   */
    int to_len; /*   char* to   */
    int cc_len; /*   char* cc   */
    int bcc_len; /*   char* bcc   */
    int attachments_count; /*   char** attachments   */
    } command_mailto;
typedef struct command_localfile
    {
    int remote_len; /*   char* remote   */
    int local_len; /*   char* local   */
    int allow_download;
    } command_localfile;
typedef struct command_uploadfile
    {
    int local_len; /*   char* local   */
    int file_len; /*   char* file   */
    int remove_local;
    } command_uploadfile;
typedef struct command_removetemporarylocalfile
    {
    int local_len; /*   char* local   */
    } command_removetemporarylocalfile;
typedef struct reply_init
    {
    int ok;
    } reply_init;
typedef struct reply_openurl
    {
    int ok;
    } reply_openurl;
typedef struct reply_executeurl
    {
    int ok;
    } reply_executeurl;
typedef struct reply_buttonorder
    {
    int order;
    } reply_buttonorder;
typedef struct reply_runasuser
    {
    int ok;
    } reply_runasuser;
typedef struct reply_suspendscreensaving
    {
    int ok;
    } reply_suspendscreensaving;
typedef struct reply_mailto
    {
    int ok;
    } reply_mailto;
typedef struct reply_localfile
    {
    int result_len; /*   char* result   */
    } reply_localfile;
typedef struct reply_uploadfile
    {
    int ok;
    } reply_uploadfile;
typedef struct reply_removetemporarylocalfile
    {
    int ok;
    } reply_removetemporarylocalfile;
