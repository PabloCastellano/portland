int openUrl( const char* url );
int suspendScreensaving( Display* dpy, int suspend );
int mailTo( const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, const char** attachments );
