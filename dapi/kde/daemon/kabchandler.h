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

#ifndef KABCHANDLER_H
#define KABCHANDLER_H

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

    bool getNames(const QString& uid, QCString& givenName, QCString& familyName,
                  QCString& fullName) const;

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
