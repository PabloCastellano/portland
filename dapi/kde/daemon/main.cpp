#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>

#include "handler.h"

int main( int argc, char* argv[] )
    {
    KCmdLineArgs::init( argc, argv, "dapi_kde", I18N_NOOP( "dapi_kde" ), I18N_NOOP( "dapi_kde" ), "0.1" );
    KApplication app;
    KDapiHandler handler;
    return app.exec();
    }
