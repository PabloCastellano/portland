#ifndef HANDLER_H
#define HANDLER_H

#include <qobject.h>
#include <qmap.h>
#include <kio/job.h>
#include <qwidget.h>

#include <dapi/comm.h>

class QSocketNotifier;

class KDapiHandler
    : public QObject
    {
    Q_OBJECT
    public:
        KDapiHandler();
        virtual ~KDapiHandler();
    private slots:
        void processMainSocketData();
        void processSocketData( int sock );
    private:
        struct ConnectionData
            {
            DapiConnection* conn;
            QSocketNotifier* notifier;
            bool screensaver_suspend;
            };
        void setupSocket();
        void closeSocket( ConnectionData& conn );
        void processCommand( ConnectionData& conn );
        void processCommandInit( ConnectionData& conn, int seq );
        void processCommandCapabilities( ConnectionData& conn, int seq );
        void processCommandOpenUrl( ConnectionData& conn, int seq );
        void processCommandExecuteUrl( ConnectionData& conn, int seq );
        void processCommandButtonOrder( ConnectionData& conn, int seq );
        void processCommandRunAsUser( ConnectionData& conn, int seq );
        void processCommandSuspendScreensaving( ConnectionData& conn, int seq );
        void processCommandMailTo( ConnectionData& conn, int seq );
        void processCommandLocalFile( ConnectionData& conn, int seq );
        void processCommandUploadFile( ConnectionData& conn, int seq );
        void processCommandRemoveTemporaryLocalFile( ConnectionData& conn, int seq );
        void updateScreensaving();
        static QCString makeStartupInfo( const DapiWindowInfo& winfo );
        int mainsocket;
        typedef QValueList< ConnectionData > ConnectionList;
        ConnectionList connections;
    };

class KDapiFakeWidget
    : public QWidget
    {
    Q_OBJECT
    public:
        KDapiFakeWidget( WId window );
        virtual ~KDapiFakeWidget();
    };

class KDapiDownloadJob
    : public QObject
    {
    Q_OBJECT
    public:
        KDapiDownloadJob( KIO::FileCopyJob* j, DapiConnection* c, int s, QWidget* w );
    private slots:
        void done();
    private:
        KIO::FileCopyJob* job;
        DapiConnection* conn;
        int seq;
        QWidget* widget;
    };

class KDapiUploadJob
    : public QObject
    {
    Q_OBJECT
    public:
        KDapiUploadJob( KIO::FileCopyJob* j, DapiConnection* c, int s, bool r, QWidget* w );
    private slots:
        void done();
    private:
        KIO::FileCopyJob* job;
        DapiConnection* conn;
        int seq;
        bool remove_local;
        QWidget* widget;
    };

inline
KDapiDownloadJob::KDapiDownloadJob( KIO::FileCopyJob* j, DapiConnection* c, int s, QWidget* w )
    : job( j ), conn( c ), seq( s ), widget( w )
    {
    }

inline
KDapiUploadJob::KDapiUploadJob( KIO::FileCopyJob* j, DapiConnection* c, int s, bool r, QWidget* w )
    : job( j ), conn( c ), seq( s ), remove_local( r ), widget( w )
    {
    }

#endif
