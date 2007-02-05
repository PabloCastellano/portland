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
#include <dcopref.h>
#include <kapplication.h>
#include <kglobalsettings.h>
#include <kprocess.h>
#include <krun.h>
#include <kurl.h>
#include <kio/netaccess.h>

// Qt D-Bus includes
#include <dbus/qdbusdata.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusvariant.h>

// DAPI common includes
#include "dapi-common.h"

// local includes
#include "dapiimpl.h"
#include "kabchandler.h"

#ifdef HAVE_DPMS
#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#endif

static WId dapiWindowID(const QDBusVariant& windowInfo)
{
#if defined(Q_WS_X11)
    WId window = 0;
    if (windowInfo.signature == "u")
    {
        window = windowInfo.value.toUInt32();
    }
    else if (windowInfo.signature == "t")
    {
        window = windowInfo.value.toUInt64();
    }

    return window;
#endif
}

static QCString dapiMakeStartupInfo( const QDBusVariant& windowInfo )
{
#if defined(Q_WS_X11)
    WId window = dapiWindowID(windowInfo);

    if( window != 0 )
    {
        // TODO for the window !=0 case KStartupInfo API needs to be
        // extended to accept external timestamp for creating new startup info
        KApplication::kApplication()->updateUserTimestamp();
    }
    else
    {
        KApplication::kApplication()->updateUserTimestamp();
    }
#endif

    return KStartupInfo::createNewStartupId();
}

///////////////////////////////////////////////////////////////////////////////

DAPIImpl::DAPIImpl() : org::freedesktop::dapi(),
    m_addressBook(0), m_proxyForBus(0)
{
    m_proxyForBus = new QDBusProxy(this, "ProxyForBus");

    m_proxyForBus->setService("org.freedesktop.DBus");
    m_proxyForBus->setInterface("org.freedesktop.DBus");
    m_proxyForBus->setPath("/org/freedesktop/DBus");

    QObject::connect(m_proxyForBus, SIGNAL(dbusSignal(const QDBusMessage&)),
                     this, SLOT(slotDBusSignal(const QDBusMessage&)));
}

///////////////////////////////////////////////////////////////////////////////

DAPIImpl::~DAPIImpl()
{
    delete m_addressBook;
}

///////////////////////////////////////////////////////////////////////////////

void DAPIImpl::setConnection(const QDBusConnection& connection)
{
    m_connection = connection;

    m_proxyForBus->setConnection(connection);
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
    capabilities |= DAPI_CAP_SUSPENDSCREENSAVING;
    capabilities |= DAPI_CAP_MAILTO;
    capabilities |= DAPI_CAP_LOCALFILE;
    capabilities |= DAPI_CAP_UPLOADFILE;
    capabilities |= DAPI_CAP_REMOVETEMPORARYLOCALFILE;
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

    KURL realURL = KURL::fromPathOrURL(url);

    if (!realURL.isValid())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           QString("Parameter 'url'(%1) malformed").arg(url));
        return false;
    }

    QCString startupInfo = dapiMakeStartupInfo(windowinfo);

    KApplication::kApplication()->invokeBrowser(url, startupInfo);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::ExecuteUrl(const QString& url, const QDBusVariant& windowinfo,
                          QDBusError& error)
{
    Q_UNUSED(windowinfo);

    KURL realURL = KURL::fromPathOrURL(url);

    if (!realURL.isValid())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           QString("Parameter 'url'(%1) malformed").arg(url));
        return false;
    }

    KRun* run = new KRun(url);

    WId windowID = dapiWindowID(windowinfo);
    if (windowID != 0)
    {
        KDapiFakeWidget* widget = new KDapiFakeWidget(windowID);

        QObject::connect(run, SIGNAL(destroyed()), widget, SLOT(deleteLater()));
    }

    if (run->hasError())
    {
        error = QDBusError("org.freedesktop.DBus.Error.Failed",
                           QString("Executing (%1) failed").arg(url));
        return false;
    }

    return true;
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

    if (user.isEmpty() || command.isEmpty())
    {
        QString message("Parameter 'user'(%1) or paramerter 'command' empty");
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           message.arg(user).arg(command));

        return false;
    }

    KProcess proc; // TODO use windowinfo
    proc.setUseShell(true); // TODO quoting

    proc << "kdesu";
    proc << "-u" << user;
    proc << "--" << command;

    if (proc.start(KProcess::DontCare))
    {
        QString message("Executing command (%1) failed. Exit status (%2)");
        error = QDBusError("org.freedesktop.DBus.Error.Failed",
                           message.arg(command).arg(proc.exitStatus()));

        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::SuspendScreenSaving(Q_UINT32 client_id, bool suspend,
                                   QDBusError& error)
{
    Q_UNUSED(client_id);
    Q_UNUSED(error);

    if (suspend)
        m_screenSaverSuspendConnections.insert(m_currentMethodCall.sender(), true);
    else
        m_screenSaverSuspendConnections.remove(m_currentMethodCall.sender());

    updateScreenSaverSuspend();

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::MailTo(const QString& subject, const QString& body,
                      const QString& to, const QString& cc, const QString& bcc,
                      const QStringList& attachments,
                      const QDBusVariant& windowinfo, QDBusError& error)
{
    Q_UNUSED(error);
    Q_UNUSED(windowinfo);

    QCString startupInfo = dapiMakeStartupInfo(windowinfo);

    KApplication::kApplication()->invokeMailer(to, cc, bcc, subject, body,
                                               QString::null, attachments,
                                               startupInfo);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::LocalFile(const QString& url, const QString& local,
                         bool allow_download, QString& filename,
                         const QDBusVariant& windowinfo, QDBusError& error)
{
    // save sender since downloading will allow other calls to come in
    QString sender = m_currentMethodCall.sender();

    KURL realURL = KURL::fromPathOrURL(url);

    if (!realURL.isValid())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           QString("Parameter 'url'(%1) malformed").arg(url));
        return false;
    }

    if (realURL.isLocalFile())
    {
        filename = realURL.path();
    }
    else if (!allow_download)
    {
        error = QDBusError("org.freedesktop.DBus.Error.Failed",
                           "Parameter 'url' points to remote file, "
                           "but downloading is not allowed");
        return false;
    }
    else
    {
        QWidget* widget = 0;
        WId windowID = dapiWindowID(windowinfo);
        if (windowID != 0)
        {
            widget = new KDapiFakeWidget(windowID);
        }

        // initialize download target filename
        filename = local;

        bool result = KIO::NetAccess::download(realURL, filename, widget);

        delete widget;

        if (!result)
        {
            error = QDBusError("org.freedesktop.DBus.Error.Failed",
                               KIO::NetAccess::lastErrorString());

            return false;
        }

        // if the passed filename is empty, NetAccess has created a temporary file
        if (local.isEmpty()) m_temporaryFiles.insert(filename, sender);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::UploadFile(const QString& local, const QString& url,
                          bool remove_local, const QDBusVariant& windowinfo,
                          QDBusError& error)
{
    KURL localURL = KURL::fromPathOrURL(local);
    if (!localURL.isValid())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           QString("Parameter 'local'(%1) malformed").arg(local));
        return false;
    }

    if (!localURL.isLocalFile())
    {
        QString message("Parameter 'local'(%1) is not a local file");
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           message.arg(local));
        return false;
    }

    KURL realURL  = KURL::fromPathOrURL(url);

    if (!realURL.isValid())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           QString("Parameter 'url'(%1) malformed").arg(url));
        return false;
    }

    if (realURL.isLocalFile() && localURL.pathOrURL() == realURL.pathOrURL())
    {
        // source and destination actually the same file
        return true;
    }
    else
    {
        QWidget* widget = 0;
        WId windowID = dapiWindowID(windowinfo);
        if (windowID != 0)
        {
            widget = new KDapiFakeWidget(windowID);
        }

        bool result = false;

        if (remove_local)
            result = KIO::NetAccess::move(localURL, realURL, widget);
        else
            result = KIO::NetAccess::upload(localURL.pathOrURL(), realURL, widget);

        delete widget;

        if (!result)
        {
            error = QDBusError("org.freedesktop.DBus.Error.Failed",
                               KIO::NetAccess::lastErrorString());

            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::RemoveTemporaryLocalFile(const QString& local, QDBusError& error)
{
    QMap<QString, QString>::const_iterator it = m_temporaryFiles.find(local);
    if (it == m_temporaryFiles.end())
    {
        QString message("Parameter 'local' (%1) is not a valid temporary file");

        error = QDBusError("org.freedesktop.DBus.Error.FileNotFound",
                           message.arg(local));
        return false;
    }

    if (it.data() != m_currentMethodCall.sender())
    {
        QString message("Temporary file (%1) owned by connection (%2)");
        error = QDBusError("org.freedesktop.DBus.Error.AcessDenied",
                           message.arg(local).arg(it.data()));
        return false;
    }

    m_temporaryFiles.remove(local);
    KIO::NetAccess::removeTempFile(local);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookList(QStringList& contact_ids,
                               QDBusError& error)
{
    Q_UNUSED(error);

    contact_ids = addressBook()->listUIDs();

    return true;
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
    if (name.isEmpty())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           "Parameter 'name' is empty");
        return false;
    }

    contact_ids = addressBook()->findByName(name);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetName(const QString& contact_id, QString& given_name,
                                  QString& family_name, QString& full_name,
                                  QDBusError& error)
{
    if (contact_id.isEmpty())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           "Parameter 'contact_id' is empty");
        return false;
    }

    return addressBook()->getNames(contact_id, given_name, family_name, full_name);
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetEmails(const QString& contact_id,
                                    QStringList& email_addresses,
                                    QDBusError& error)
{
    if (contact_id.isEmpty())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           "Parameter 'contact_id' is empty");
        return false;
    }

    return addressBook()->getEmails(contact_id, email_addresses);
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::AddressBookGetVCard30(const QString& contact_id, QString& vcard,
                                     QDBusError& error)
{
    if (contact_id.isEmpty())
    {
        error = QDBusError("org.freedesktop.DBus.Error.InvalidArgs",
                           "Parameter 'contact_id' is empty");
        return false;
    }

    bool found = false;
    vcard = addressBook()->vcard30(contact_id, found);

    if (found && vcard.isEmpty())
    {
        QString message("Convering data for 'contact_id'(%1) to VCard 3.0 failed");
        error = QDBusError("org.freedesktop.DBus.Error.Failed",
                           message.arg(contact_id));
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void DAPIImpl::handleMethodReply(const QDBusMessage& reply)
{
    m_connection.send(reply);
}

///////////////////////////////////////////////////////////////////////////////

bool DAPIImpl::handleMethodCall(const QDBusMessage& message)
{
    QString call = message.interface() + "." + message.member();
    qDebug("Received method call '%s' from '%s'",
           call.latin1(), message.sender().latin1());
    // some method calls need the context data from the call, e.g.
    // SuspendScreenSaving needs the sender name, so we store the the
    // message before calling the generated method handler
    m_currentMethodCall = message;

    return org::freedesktop::dapi::handleMethodCall(message);
}

///////////////////////////////////////////////////////////////////////////////

KABCHandler* DAPIImpl::addressBook()
{
    if (m_addressBook == 0)
        m_addressBook = new KABCHandler(0, "DAPI Addressbook Handler");

    return m_addressBook;
}

///////////////////////////////////////////////////////////////////////////////

void DAPIImpl::updateScreenSaverSuspend()
{
    // as long as there are connections left which have requested suspend
    bool suspend = !m_screenSaverSuspendConnections.isEmpty();

    // TODO get default state from KDE config
#ifdef HAVE_DPMS
    if( suspend )
        DPMSDisable(qt_xdisplay());
    else
        DPMSEnable(qt_xdisplay());
#endif
    DCOPRef ref( "kdesktop", "KScreensaverIface" );
    ref.call( "enable", !suspend );
}

///////////////////////////////////////////////////////////////////////////////

void DAPIImpl::slotDBusSignal(const QDBusMessage& message)
{
    if (message.sender() != m_proxyForBus->service()) return;
    if (message.member() != "NameOwnerChanged") return;

    QString name     = message[0].toString();
    QString oldOwner = message[1].toString();
    QString newOwner = message[2].toString();

    // if a unique name "gets lost" it means the connection has been closed
    if (newOwner.isEmpty() && name.startsWith(":") && name == oldOwner)
    {
        m_screenSaverSuspendConnections.remove(oldOwner);
        updateScreenSaverSuspend();
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// some KDE APIs accept only QWidget* instead of WId

KDapiFakeWidget::KDapiFakeWidget(WId window)
{
    create(window, false); // don't init
}

///////////////////////////////////////////////////////////////////////////////

KDapiFakeWidget::~KDapiFakeWidget()
{
    destroy(false); // no cleanup
}

// End of File
