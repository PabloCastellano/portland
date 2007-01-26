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

// local includes
#include "dapidaemon.h"
#include "dapiimpl.h"

///////////////////////////////////////////////////////////////////////////////

DAPIDaemon::DAPIDaemon()
    : m_dapiInterface(new DAPIImpl())
{
}

///////////////////////////////////////////////////////////////////////////////

DAPIDaemon::~DAPIDaemon()
{
    m_connection.unregisterObject("/");

    delete m_dapiInterface;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIDaemon::initConnection()
{
    m_connection = QDBusConnection::addConnection(QDBusConnection::SessionBus);

    m_dapiInterface->setConnection(m_connection);

    m_connection.registerObject("/", m_dapiInterface);

    if (!m_connection.isConnected()) return false;

    return m_connection.requestName("org.freedesktop.dapi");
}

// End of File
