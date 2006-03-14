#include <config.h>

#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <X11/Xlib.h>

#ifdef HAVE_DPMS
#include <X11/extensions/dpms.h>
#endif

#include "commands.h"

static void sigchld( int s )
    {
    int status;
    ( void ) s;
    while( waitpid( -1, &status, WNOHANG ) > 0 )
        ;
    }

int openUrl( const char* url )
    {
    struct sigaction sa, saold;
    sa.sa_handler = sigchld;
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    sigemptyset( &sa.sa_mask );
    sigaction( SIGCHLD, &sa, &saold );
    if( fork() == 0 )
        {
        const char* browser = getenv( "BROWSER" );
        if( browser != NULL )
            execlp( browser, browser, url, NULL );
        execlp( "firefox", "firefox", url, NULL );
        execlp( "mozilla", "mozilla", url, NULL );
        execlp( "netscape", "netscape", url, NULL );
        execlp( "opera", "opera", url, NULL );
        exit( 1 );
        }
    return 1; /* TODO */
    }

int suspendScreensaving( Display* dpy, int suspend )
    {
#ifdef HAVE_DPMS
    if( !DPMSCapable( dpy ))
        return 0;
    if( suspend )
        return DPMSDisable( dpy );
    else
        return DPMSEnable( dpy );
#else
    return 0;
#endif
    }

int mailTo( const char* subject, const char* body, const char* to,
    const char* cc, const char* bcc, const char** attachments, int att_count )
    {
    ( void ) subject;
    ( void ) body;
    ( void ) to;
    ( void ) cc;
    ( void ) bcc;
    ( void ) attachments;
    ( void ) att_count;
    /* TODO */
    return 0;
    }
