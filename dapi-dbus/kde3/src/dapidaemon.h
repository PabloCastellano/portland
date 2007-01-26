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

#if !defined(DAPIDAEMON_H_INCLUDED)
#define DAPIDAEMON_H_INCLUDED

// Qt D-Bus includes
#include <dbus/qdbusconnection.h>

// forward declarations
class DAPIImpl;

class DAPIDaemon
{
public:
    DAPIDaemon();

    ~DAPIDaemon();

    bool initConnection();

private:
    QDBusConnection m_connection;

    DAPIImpl* m_dapiInterface;
};

#endif

// End of File
