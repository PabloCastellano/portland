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

#if !defined(DAPIIMPL_H_INCLUDED)
#define DAPIIMPL_H_INCLUDED

// Qt includes
#include <qmap.h>
#include <qwidget.h>

// Qt D-Bus includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>

// local includes
#include "dapi.h"

// forward declarations
class KABCHandler;
class QDBusProxy;

typedef QMap<QString, bool> StringSet;

class DAPIImpl : public QObject, public org::freedesktop::dapi
{
    Q_OBJECT
public:
    DAPIImpl();

    virtual ~DAPIImpl();

    void setConnection(const QDBusConnection& connection);

protected:
    QDBusConnection m_connection;

    KABCHandler* m_addressBook;

protected:
    virtual bool Capabilities(Q_INT32& capabilities, QDBusError& error);

    virtual bool OpenUrl(const QString& url, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool ExecuteUrl(const QString& url, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool ButtonOrder(Q_INT32& order, QDBusError& error);

    virtual bool RunAsUser(const QString& user, const QString& command, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool SuspendScreenSaving(Q_UINT32 client_id, bool suspend, QDBusError& error);

    virtual bool MailTo(const QString& subject, const QString& body, const QString& to, const QString& cc, const QString& bcc, const QStringList& attachments, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool LocalFile(const QString& url, const QString& local, bool allow_download, QString& filename, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool UploadFile(const QString& local, const QString& url, bool remove_local, const QDBusVariant& windowinfo, QDBusError& error);

    virtual bool RemoveTemporaryLocalFile(const QString& local, QDBusError& error);

    virtual bool AddressBookList(QStringList& contact_ids, QDBusError& error);

    virtual bool AddressBookOwner(QString& contact_id, QDBusError& error);

    virtual bool AddressBookFindByName(const QString& name, QStringList& contact_ids, QDBusError& error);

    virtual bool AddressBookGetName(const QString& contact_id, QString& given_name, QString& family_name, QString& full_name, QDBusError& error);

    virtual bool AddressBookGetEmails(const QString& contact_id, QStringList& email_addresses, QDBusError& error);

    virtual bool AddressBookGetVCard30(const QString& contact_id, QString& vcard, QDBusError& error);

    virtual void handleMethodReply(const QDBusMessage& reply);

    virtual bool handleMethodCall(const QDBusMessage& message);

protected:
    KABCHandler* addressBook();

private:
    QDBusProxy* m_proxyForBus;

    StringSet m_screenSaverSuspendConnections;

    QMap<QString, QString> m_temporaryFiles;

    QDBusMessage m_currentMethodCall;

private:
    void updateScreenSaverSuspend();

private slots:
    void slotDBusSignal(const QDBusMessage& message);
};

class KDapiFakeWidget : public QWidget
{
    Q_OBJECT
public:
    KDapiFakeWidget( WId window );
    virtual ~KDapiFakeWidget();
};

#endif

// End of File
