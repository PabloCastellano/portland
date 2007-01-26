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
#include "kabchandler.h"

// KABC includes
#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/stdaddressbook.h>
#include <kabc/vcardconverter.h>

using namespace KABC;

///////////////////////////////////////////////////////////////////////////////

KABCHandler::KABCHandler(QObject* parent, const char* name)
    : QObject(parent, name),
      m_addressBook(0),
      m_vcardConverter(0)
{
    m_addressBook = StdAddressBook::self(false);
    QObject::connect(m_addressBook, SIGNAL(addressBookChanged(AddressBook*)),
                     this, SLOT(slotAddressBookChanged()));

    m_vcardConverter = new VCardConverter();
}

///////////////////////////////////////////////////////////////////////////////

KABCHandler::~KABCHandler()
{
    delete m_vcardConverter;
}

///////////////////////////////////////////////////////////////////////////////

QStringList KABCHandler::listUIDs() const
{
    AddressBook::ConstIterator it    = m_addressBook->begin();
    AddressBook::ConstIterator endIt = m_addressBook->end();

    QStringList uids;
    for (; it != endIt; ++it)
    {
        uids << (*it).uid();
    }

    return uids;
}

///////////////////////////////////////////////////////////////////////////////

bool KABCHandler::getNames(const QString& uid, QString& givenName,
                           QString& familyName, QString& fullName) const
{
    Addressee contact = m_addressBook->findByUid(uid);

    if (contact.isEmpty()) return false;

    givenName  = contact.givenName();
    familyName = contact.familyName();
    fullName   = contact.assembledName();

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool KABCHandler::getEmails(const QString& uid, QStringList& emails) const
{
    Addressee contact = m_addressBook->findByUid(uid);

    if (contact.isEmpty()) return false;

    emails = contact.emails();

    return true;
}

///////////////////////////////////////////////////////////////////////////////

QStringList KABCHandler::findByName(const QString& name) const
{
    AddressBook::ConstIterator it    = m_addressBook->begin();
    AddressBook::ConstIterator endIt = m_addressBook->end();

    QStringList uids;
    for (; it != endIt; ++it)
    {
        if (hasNameMatch(*it, name.lower()))
            uids << (*it).uid();
    }

    return uids;
}

///////////////////////////////////////////////////////////////////////////////

QString KABCHandler::owner() const
{
    return m_addressBook->whoAmI().uid();
}

///////////////////////////////////////////////////////////////////////////////

QString KABCHandler::vcard30(const QString& uid) const
{
    Addressee contact = m_addressBook->findByUid(uid);

    if (contact.isEmpty()) return QString::null;

    return m_vcardConverter->createVCard(contact, VCardConverter::v3_0);
}

///////////////////////////////////////////////////////////////////////////////

bool KABCHandler::hasNameMatch(const KABC::Addressee& contact, const QString& name)
{
    if (contact.assembledName().lower().find(name) != -1) return true;

    if (contact.formattedName().lower().find(name) != -1) return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////

void KABCHandler::slotAddressBookChanged()
{
    qDebug("AddressBook changed");
}

#include "kabchandler.moc"

// End of File
