#ifndef HANDLER_H
#define HANDLER_H

#include <qobject.h>
#include <qmap.h>

#include "comm.h"

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

#endif
