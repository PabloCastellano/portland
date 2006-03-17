/******************************************************************************
    Copyright 2006, Kevin Krammer <kevin.krammer@gmx.at>
    All Rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
    m_addressBook = StdAddressBook::self(true);
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

bool KABCHandler::getNames(const QString& uid, QCString& givenName,
                           QCString& familyName, QCString& fullName) const
{
    Addressee contact = m_addressBook->findByUid(uid);

    if (contact.isEmpty()) return false;

    givenName  = contact.givenName().utf8();
    familyName = contact.familyName().utf8();
    fullName   = contact.assembledName().utf8();

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
