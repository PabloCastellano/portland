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
#include <kapplication.h>
#include <kglobalsettings.h>
#include <kprocess.h>
#include <krun.h>

// Qt D-Bus includes
#include <dbus/qdbusdata.h>
#include <dbus/qdbuserror.h>

// DAPI common includes
#include "dapi-common.h"

// local includes
#include "dapiimpl.h"
#include "kabchandler.h"

///////////////////////////////////////////////////////////////////////////////

DAPIImpl::DAPIImpl() : org::freedesktop::dapi(),
    m_addressBook(0)
{
}

///////////////////////////////////////////////////////////////////////////////

DAPIImpl::~DAPIImpl()
{
    delete m_addressBook;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::Capabilities(Q_INT32& capabilities, QDBusError& error)
{
    Q_UNUSED(error);

    capabilities = 0;

    capabilities |= DAPI_CAP_OPENURL;
    capabilities |= DAPI_CAP_EXECUTEURL;
    capabilities |= DAPI_CAP_BUTTONORDER;
    capabilities |= DAPI_CAP_RUNASUSER;
    //capabilities |= DAPI_CAP_SUSPENDSCREENSAVING;
    capabilities |= DAPI_CAP_MAILTO;
    //capabilities |= DAPI_CAP_LOCALFILE;
    //capabilities |= DAPI_CAP_UPLOADFILE;
    //capabilities |= DAPI_CAP_REMOVETEMPORARYLOCALFILE;
    capabilities |= DAPI_CAP_ADDRESSBOOKLIST;
    capabilities |= DAPI_CAP_ADDRESSBOOKGETNAME;
    capabilities |= DAPI_CAP_ADDRESSBOOKGETEMAILS;
    capabilities |= DAPI_CAP_ADDRESSBOOKFINDBYNAME;
    capabilities |= DAPI_CAP_ADDRESSBOOKOWNER;
    capabilities |= DAPI_CAP_ADDRESSBOOKGETVCARD30;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::OpenUrl(const QString& url, const QDBusVariant& windowinfo,
                       QDBusError& error)
{
    Q_UNUSED(windowinfo);

    error = QDBusError("org.freedesktop.dapi.Error", "OpenUrl failed");

    if (url.isEmpty()) return false;

    KApplication::kApplication()->invokeBrowser(url); // TODO use windowinfo

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::ExecuteUrl(const QString& url, const QDBusVariant& windowinfo,
                          QDBusError& error)
{
    Q_UNUSED(windowinfo);

    error = QDBusError("org.freedesktop.dapi.Error", "ExecuteUrl failed");

    if (url.isEmpty()) return false;

    KRun* run = new KRun(url); // TODO use windowinfo

    return !run->hasError();
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::ButtonOrder(Q_INT32& order, QDBusError& error)
{
    Q_UNUSED(error);

    order = KGlobalSettings::buttonLayout();
    // TODO KDE has actually more layouts, but I have no idea what
    // they're supposed to mean

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::RunAsUser(const QString& user, const QString& command,
                         const QDBusVariant& windowinfo, QDBusError& error)
{
    Q_UNUSED(windowinfo);

    error = QDBusError("org.freedesktop.dapi.Error", "RunAsUser failed");

    if (user.isEmpty() || command.isEmpty()) return false;

    KProcess proc; // TODO use windowinfo
    proc.setUseShell(true); // TODO quoting

    proc << "kdesu";
    proc << "-u" << user;
    proc << "--" << command;

    return proc.start( KProcess::DontCare );
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::SuspendScreenSaving(Q_UINT32 client_id, bool suspend,
                                   QDBusError& error)
{
    Q_UNUSED(client_id);
    Q_UNUSED(suspend);

    error = QDBusError("org.freedesktop.dapi.Error", "NotImplementedYet");

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::MailTo(const QString& subject, const QString& body,
                      const QString& to, const QString& cc, const QString& bcc,
                      const QStringList& attachments,
                      const QDBusVariant& windowinfo, QDBusError& error)
{
    Q_UNUSED(error);
    Q_UNUSED(windowinfo);

    // TODO use windowinfo
    KApplication::kApplication()->invokeMailer(to, cc, bcc, subject, body,
                                               QString::null, attachments);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::LocalFile(const QString& url, const QString& local,
                         bool allow_download, QString& filename,
                         const QDBusVariant& windowinfo, QDBusError& error)
{
    Q_UNUSED(url);
    Q_UNUSED(local);
    Q_UNUSED(allow_download);
    Q_UNUSED(filename);
    Q_UNUSED(windowinfo);

    error = QDBusError("org.freedesktop.dapi.Error", "NotImplementedYet");

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::UploadFile(const QString& local, const QString& url,
                          bool remove_local, const QDBusVariant& windowinfo,
                          QDBusError& error)
{
    Q_UNUSED(local);
    Q_UNUSED(url);
    Q_UNUSED(remove_local);
    Q_UNUSED(windowinfo);

    error = QDBusError("org.freedesktop.dapi.Error", "NotImplementedYet");

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::RemoveTemporaryLocalFile(const QString& local, QDBusError& error)
{
    Q_UNUSED(local);

    error = QDBusError("org.freedesktop.dapi.Error", "NotImplementedYet");

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookList(QStringList& contact_ids,
                               QDBusError& error)
{
    error = QDBusError("org.freedesktop.dapi.Error", "AddressBookList failed");

    contact_ids = addressBook()->listUIDs();

    return !contact_ids.isEmpty();
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookOwner(QString& contact_id, QDBusError& error)
{
    Q_UNUSED(error);

    contact_id = addressBook()->owner();

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookFindByName(const QString& name,
                                     QStringList& contact_ids,
                                     QDBusError& error)
{
    error = QDBusError("org.freedesktop.dapi.Error",
                       "AddressBookFindByName failed");

    if (name.isEmpty()) return false;

    contact_ids = addressBook()->findByName(name);

    return !contact_ids.isEmpty();
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetName(const QString& contact_id, QString& given_name,
                                  QString& family_name, QString& full_name,
                                  QDBusError& error)
{
    error = QDBusError("org.freedesktop.dapi.Error", "AddressBookGetName failed");

    if (contact_id.isEmpty()) return false;

    return addressBook()->getNames(contact_id, given_name, family_name, full_name);
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetEmails(const QString& contact_id,
                                    QStringList& email_addresses,
                                    QDBusError& error)
{
    error = QDBusError("org.freedesktop.dapi.Error", "AddressBookGetEmails failed");

    if (contact_id.isEmpty()) return false;

    return addressBook()->getEmails(contact_id, email_addresses);
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetVCard30(const QString& contact_id, QString& vcard,
                                     QDBusError& error)
{
    error = QDBusError("org.freedesktop.dapi.Error",
                       "AddressBookGetVCarf30 failed");

    if (contact_id.isEmpty()) return false;

    vcard = addressBook()->vcard30(contact_id);

    return !vcard.isEmpty();
}

///////////////////////////////////////////////////////////////////////////////

void DAPIImpl::handleMethodReply(const QDBusMessage& reply)
{
    m_connection.send(reply);
}

///////////////////////////////////////////////////////////////////////////////

KABCHandler* DAPIImpl::addressBook()
{
    if (m_addressBook == 0)
        m_addressBook = new KABCHandler(0, "DAPI Addressbook Handler");

    return m_addressBook;
}

// End of File
