#include <config.h>

#include "handler.h"
#include "handler.moc"

#include <dcopref.h>
#include <qsocketnotifier.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kio/netaccess.h>
#include <kprocess.h>
#include <krun.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#ifdef HAVE_DPMS
#include <X11/extensions/dpms.h>
#endif

KDapiHandler::KDapiHandler()
    {
    setupSocket();
    }

KDapiHandler::~KDapiHandler()
    {
    while( !connections.isEmpty())
        closeSocket( *connections.begin());
    }

void KDapiHandler::setupSocket()
    {
    mainsocket = dapi_bindSocket();
    if( mainsocket < 0 )
        return;
    QSocketNotifier* notif = new QSocketNotifier( mainsocket, QSocketNotifier::Read, this );
    connect( notif, SIGNAL( activated( int )), SLOT( processMainSocketData()));
    }

void KDapiHandler::processMainSocketData()
    {
    DapiConnection* conn = dapi_acceptSocket( mainsocket );
    if( conn == NULL )
        return;
    ConnectionData data;
    data.conn = conn;
    data.notifier = new QSocketNotifier( dapi_socket( data.conn ), QSocketNotifier::Read, this );
    connect( data.notifier, SIGNAL( activated( int )), SLOT( processSocketData( int )));
    data.screensaver_suspend = false;
    connections.append( data );
    }

void KDapiHandler::processSocketData( int sock )
    {
    for( ConnectionList::Iterator it = connections.begin();
         it != connections.end();
         ++it )
        if( dapi_socket((*it).conn ) == sock )
            {
            processCommand( *it );
            break;
            }
    }
    
void KDapiHandler::processCommand( ConnectionData& conn )
    {
    int command;
    int seq;
    if( !dapi_readCommand( conn.conn, &command, &seq ))
        {
        closeSocket( conn );
        return;
        }
    switch( command )
        {
        case DAPI_COMMAND_INIT:
            processCommandInit( conn, seq );
            return;
        case DAPI_COMMAND_OPENURL:
            processCommandOpenUrl( conn, seq );
            return;
        case DAPI_COMMAND_EXECUTEURL:
            processCommandExecuteUrl( conn, seq );
            return;
        case DAPI_COMMAND_BUTTONORDER:
            processCommandButtonOrder( conn, seq );
            return;
        case DAPI_COMMAND_RUNASUSER:
            processCommandRunAsUser( conn, seq );
            return;
        case DAPI_COMMAND_SUSPENDSCREENSAVING:
            processCommandSuspendScreensaving( conn, seq );
            return;
        case DAPI_COMMAND_MAILTO:
            processCommandMailTo( conn, seq );
            return;
        case DAPI_COMMAND_LOCALFILE:
            processCommandLocalFile( conn, seq );
            return;
        case DAPI_COMMAND_UPLOADFILE:
            processCommandUploadFile( conn, seq );
            return;
        case DAPI_COMMAND_REMOVETEMPORARYLOCALFILE:
            processCommandRemoveTemporaryLocalFile( conn, seq );
            return;
        }
    }

void KDapiHandler::closeSocket( ConnectionData& conn )
    {
    dapi_close( conn.conn );
    delete conn.notifier;
    for( ConnectionList::Iterator it = connections.begin();
         it != connections.end();
         ++it )
        if( (*it).conn == conn.conn )
            {
            connections.remove( it );
            break;
            }
    updateScreensaving();
    }

void KDapiHandler::processCommandInit( ConnectionData& conn, int seq )
    {
    if( !dapi_readCommandInit( conn.conn ))
        {
        closeSocket( conn );
        return;
        }
    dapi_writeReplyInit( conn.conn, seq, 1 );
    }

void KDapiHandler::processCommandOpenUrl( ConnectionData& conn, int seq )
    {
    char* url;
    if( !dapi_readCommandOpenUrl( conn.conn, &url ))
        {
        closeSocket( conn );
        return;
        }
    kapp->invokeBrowser( url );
    dapi_writeReplyOpenUrl( conn.conn, seq, 1 );
    free( url );
    }

void KDapiHandler::processCommandExecuteUrl( ConnectionData& conn, int seq )
    {
    char* url;
    if( !dapi_readCommandExecuteUrl( conn.conn, &url ))
        {
        closeSocket( conn );
        return;
        }
    new KRun( url );
    dapi_writeReplyExecuteUrl( conn.conn, seq, 1 );
    free( url );
    }

void KDapiHandler::processCommandButtonOrder( ConnectionData& conn, int seq )
    {
    if( !dapi_readCommandButtonOrder( conn.conn ))
        {
        closeSocket( conn );
        return;
        }
    int order = KGlobalSettings::buttonLayout();
    // TODO KDE has actually more layouts, but I have no idea what they're supposed to mean
    dapi_writeReplyButtonOrder( conn.conn, seq, order == 1 ? 2 : 1 );
    }

void KDapiHandler::processCommandRunAsUser( ConnectionData& conn, int seq )
    {
    char* user;
    char* command;
    if( !dapi_readCommandRunAsUser( conn.conn, &user, &command ))
        {
        closeSocket( conn );
        return;
        }
    KProcess proc;
    proc.setUseShell( true ); // TODO quoting
    proc << "kdesu";
    if( user[ 0 ] != '\0' )
        proc << "-u" << user;
    proc << "--" << command;
    bool ret = proc.start( KProcess::DontCare );
    dapi_writeReplyRunAsUser( conn.conn, seq, ret ? 1 : 0 );
    free( user );
    free( command );
    }

void KDapiHandler::processCommandSuspendScreensaving( ConnectionData& conn, int seq )
    {
    int suspend;
    if( !dapi_readCommandSuspendScreensaving( conn.conn, &suspend ))
        {
        closeSocket( conn );
        return;
        }
    conn.screensaver_suspend = suspend;
    updateScreensaving();
    dapi_writeReplySuspendScreensaving( conn.conn, seq, 1 );
    }

void KDapiHandler::updateScreensaving()
    {
    bool suspend = false;
    for( ConnectionList::ConstIterator it = connections.begin();
         it != connections.end();
         ++it )
        {
        if( (*it).screensaver_suspend )
            {
            suspend = true;
            break;
            }
        }
#ifdef HAVE_DPMS
    if( suspend )
        DPMSDisable( qt_xdisplay());
    else
        DPMSEnable( qt_xdisplay());
#endif
    DCOPRef ref( "kdesktop", "KScreensaverIface" );
    ref.call( "enable", !suspend );
    }

void KDapiHandler::processCommandMailTo( ConnectionData& conn, int seq )
    {
    char* subject;
    char* body;
    char* to;
    char* cc;
    char* bcc;
    stringarr attachments;
    if( !dapi_readCommandMailTo( conn.conn, &subject, &body, &to, &cc, &bcc, &attachments ))
        {
        closeSocket( conn );
        return;
        }
    QStringList attachurls;
    for( int i = 0;
         i < attachments.count;
         ++i )
        attachurls.append( QString::fromUtf8( attachments.data[ i ] ));
    kapp->invokeMailer( QString::fromUtf8( to ), QString::fromUtf8( cc ), QString::fromUtf8( bcc ),
        QString::fromUtf8( subject ), QString::fromUtf8( body ), QString(), attachurls );
    dapi_writeReplyMailTo( conn.conn, seq, 1 );
    free( subject );
    free( body );
    free( to );
    free( cc );
    free( bcc );
    dapi_freestringarr( attachments );
    }

void KDapiHandler::processCommandLocalFile( ConnectionData& conn, int seq )
    {
    char* file;
    char* local;
    int allow_download;
    if( !dapi_readCommandLocalFile( conn.conn, &file, &local, &allow_download ))
        {
        closeSocket( conn );
        return;
        }
    KURL url = KURL::fromPathOrURL( QString::fromUtf8( file ));
    free( file );
    QString result;
    if( !url.isValid())
        ; // result is empty
    else if( url.isLocalFile())
        result = url.path();
    else
        {
        QString target = QString::fromUtf8( local );
        // TODO use KIO directly instead of NetAccess
        if( allow_download && KIO::NetAccess::download( url, target, 0 )) // TODO window
            result = target;
        }
    dapi_writeReplyLocalFile( conn.conn, seq, result.utf8());
    }

void KDapiHandler::processCommandUploadFile( ConnectionData& conn, int seq )
    {
    char* local;
    char* file;
    int remove_local;
    if( !dapi_readCommandUploadFile( conn.conn, &local, &file, &remove_local ))
        {
        closeSocket( conn );
        return;
        }
    KURL url = KURL::fromPathOrURL( QString::fromUtf8( file ));
    QString localfile = QString::fromUtf8( local );
    free( local );
    free( file );
    int ok = 0;
    if( !url.isValid())
        ok = 0;
    else if( url.isLocalFile())
        ok = 1; // no-op
    else
        {
        // TODO use KIO directly instead of NetAccess
        if( KIO::NetAccess::upload( localfile, url, 0 )) // TODO window
        // TODO this apparently returns success even with e.g. http - check somehow
            {
            if( remove_local )
                KIO::NetAccess::removeTempFile( localfile );
            ok = 1;
            }
        }
    dapi_writeReplyUploadFile( conn.conn, seq, ok );
    }

void KDapiHandler::processCommandRemoveTemporaryLocalFile( ConnectionData& conn, int seq )
    {
    char* file;
    if( !dapi_readCommandRemoveTemporaryLocalFile( conn.conn, &file ))
        {
        closeSocket( conn );
        return;
        }
    KIO::NetAccess::removeTempFile( QString::fromUtf8( file ));
    dapi_writeReplyRemoveTemporaryLocalFile( conn.conn, seq, 1 );
    free( file );
    }
