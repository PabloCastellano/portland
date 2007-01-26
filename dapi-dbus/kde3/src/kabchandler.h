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

#if !defined(KABCHANDLER_H_INCLUDED)
#define KABCHANDLER_H_INCLUDED

// Qt includes
#include <qobject.h>

// forward declarations
namespace KABC
{
    class Addressee;
    class StdAddressBook;
    class VCardConverter;
};
class QStringList;

class KABCHandler : public QObject
{
    Q_OBJECT

public:
    KABCHandler(QObject* parent = 0, const char* name = 0);
    virtual ~KABCHandler();

    QStringList listUIDs() const;

    bool getNames(const QString& uid, QString& givenName, QString& familyName,
                  QString& fullName) const;

    bool getEmails(const QString& uid, QStringList& emails) const;

    QStringList findByName(const QString& uid) const;

    QString owner() const;

    QString vcard30(const QString& uid) const;

private:
    KABC::StdAddressBook* m_addressBook;

    KABC::VCardConverter* m_vcardConverter;

private:
    static bool hasNameMatch(const KABC::Addressee& contact, const QString& name);

private slots:
    void slotAddressBookChanged();
};

#endif

// End of File
