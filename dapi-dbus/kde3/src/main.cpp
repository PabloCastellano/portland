//
//  Copyright (C) 2007 Kevin Krammer <kevin.krammer@gmx.at>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

// KDE includes
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

// local includes
#include "dapidaemon.h"

static const char version[] = "0.1";

static KCmdLineOptions cmdLineOptions[] =
{
    KCmdLineLastOption
};

int main(int argc, char** argv)
{
    KAboutData aboutData("dapi-kde", I18N_NOOP("DAPI daemon for KDE3"), version,
            I18N_NOOP("A KDE3 based implementation of the DAPI interfaces"),
            KAboutData::License_GPL_V2);

    aboutData.addAuthor("Kevin Krammer", I18N_NOOP("Primary Author"),
                        "kevin.krammer@gmx.at");

    KCmdLineArgs::addCmdLineOptions(cmdLineOptions);
    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app(true); // enable GUI, some calls might involve dialogs

    DAPIDaemon daemon;

    if (!daemon.initConnection()) return -1;

    return app.exec();
}

// End of File
