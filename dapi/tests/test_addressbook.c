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

#include <stdio.h>
#include <stdlib.h>

#include <dapi/comm.h>
#include <dapi/calls.h>

void printContact( DapiConnection* conn, int num, char* id );
void printVCard( DapiConnection* conn, char* id );

int main(int argc, char** argv)
    {
    DapiConnection* conn = dapi_connectAndInit();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }

    char* ownerID = NULL;
    if( dapi_AddressBookOwner( conn, &ownerID ) )
        {
        printf( "AddressBookOwner: contact for current user available:\n" );
        printContact( conn, 0, ownerID );
        free( ownerID );
        }
    else
        printf( "AddressBookOwner: no contact for the current user\n" );
    printf("\n");

    stringarr idlist;
    if( dapi_AddressBookList( conn, &idlist ) )
        printf( "AddressBookList: %d contact IDs\n", idlist.count );
    else
        printf( "AddressBookList: call failed\n" );

    int i;
    for( i = 0; i < idlist.count; ++i )
        printContact( conn, i, idlist.data[i] );
    dapi_freestringarr( idlist );

    if( argc <= 1 )
        printf( "\nNo commandline args, skipping FindByName\n" );
    else
        {
        for( i = 1; i < argc; ++i )
            {
                printf( "\nSearching for '%s'\n", argv[i] );

                stringarr foundlist;
                if( dapi_AddressBookFindByName( conn, argv[i], &foundlist ) )
                    {
                    if ( foundlist.count > 0 )
                        {
                        printf( "Found %d matches\n", foundlist.count );

                        int j;
                        for ( j = 0; j < foundlist.count; ++j )
                            {
                                printContact( conn, j, foundlist.data[j] );
                                printVCard( conn, foundlist.data[j] );
                            }
                        dapi_freestringarr( foundlist );
                        }
                    else
                        printf( "No matches in addressbook\n" );
                    }
                else
                    printf( "AddressBoolFindByName: call failed\n" );
            }
        }

    dapi_close( conn );

    return 0;
    }

void printContact( DapiConnection* conn, int num, char* id )
    {
    char* givenname;
    char* familyname;
    char* fullname;

    if( dapi_AddressBookGetName( conn, id, &givenname, &familyname, &fullname ) )
        {
        printf( "Contact %d: '%s', %s', '%s'\n",
                num, givenname, familyname, fullname );

        free( givenname );
        free( familyname );
        free( fullname );
        }
    else
        printf( "AddressBookGetName: call failed\n" );

    stringarr emaillist;
    if( dapi_AddressBookGetEmails( conn, id, &emaillist ) )
        {
        if( emaillist.count > 0 )
            {
            int j;
            for( j = 0; j < emaillist.count; ++j )
                {
                printf( "\temail %d: '%s'\n", j, emaillist.data[j] );
                }
            dapi_freestringarr( emaillist );
            }
        else
            printf( "\tNo emails\n" );
        }
    else
        printf( "AddressBookGetEmails: call failed\n" );
    }

void printVCard( DapiConnection* conn, char* id )
    {
    char* vcard;
    if( dapi_AddressBookGetVCard30( conn, id, &vcard ) )
        {
            printf("\n%s\n", vcard);
            free( vcard );
        }
    else
        printf( "AddressBookGetVCard30: call failed" );
    }
