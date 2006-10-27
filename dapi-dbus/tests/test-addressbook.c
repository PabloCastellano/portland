/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2006 Imendio AB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Mikael Hallendal <micke@test-user.com>
 *         Martyn Russell <martyn@test-user.com>
 */

#include <string.h>

#include <glib.h>
#include <libebook/e-book.h>

#include "tests.h"

#define d(x)

#define CONTACT_FULL_NAME   "Dapi TestUser"
#define CONTACT_GIVEN_NAME  "Dapi"
#define CONTACT_FAMILY_NAME "TestUser"
#define CONTACT_FILE_AS     "TestUser, Dapi"
#define CONTACT_EMAIL_1     "Dapi@test-user.com"
#define CONTACT_EMAIL_2     "TestUser@test-user.com"
#define CONTACT_EMAIL_3     "DapiTestUser@test-user.com"

static const gchar *new_contact_id = NULL;

/* FIXME: I have noticed that if a test fails that the post condition
 * is NEVER run, which means you end up with a lot of new contacts
 * which don't get deleted!  
 */

static void
pre_test (void)
{
	EBook       *book;
	EContact    *contact;
	EBookChange *change;
	GList       *changes;
	GError      *error = NULL;
	const gchar *str;
	gboolean     success;

	if (new_contact_id) {
		g_warning ("Contact ID is already set, it looks like "
			   "the previous test didn't clean up");
		return;
	}

	d(g_print ("\n"));

	/* Add a contact to the address book here */
	book = e_book_new_system_addressbook (NULL);
	g_return_if_fail (book != NULL);

	success = e_book_open (book, FALSE, &error);
	if (!success) {
		g_warning ("Could not open address book, %s", 
			   error ? error->message : "no error given");
		g_clear_error (&error);
		g_object_unref (book); 
		return;
	}

	contact = e_contact_new ();
	if (!contact) {
		g_warning ("Could not create new contact");
		g_object_unref (book); 
		return;
	}

	/* Set details for contact */
	e_contact_set (contact, E_CONTACT_FILE_AS, CONTACT_FILE_AS);
	e_contact_set (contact, E_CONTACT_FULL_NAME, CONTACT_FULL_NAME);
	e_contact_set (contact, E_CONTACT_GIVEN_NAME, CONTACT_GIVEN_NAME);
	e_contact_set (contact, E_CONTACT_FAMILY_NAME, CONTACT_FAMILY_NAME);
	e_contact_set (contact, E_CONTACT_EMAIL_1, CONTACT_EMAIL_1);
	e_contact_set (contact, E_CONTACT_EMAIL_2, CONTACT_EMAIL_2);
	e_contact_set (contact, E_CONTACT_EMAIL_3, CONTACT_EMAIL_3);

	success = e_book_add_contact (book, contact, &error);
	if (!success) {
		g_warning ("Could not add new contact, %s", 
			   error ? error->message : "no error given");
		g_clear_error (&error);
		g_object_unref (book); 
		return;
	}

	if (!e_book_get_changes (book, "changeidtest", &changes, &error)) {
		g_warning ("Could not get book changes, %s", 
			   error ? error->message : "no error given");
		g_object_unref (contact); 
		g_object_unref (book); 
		return;
	}

	change = changes->data;
	if (change->change_type != E_BOOK_CHANGE_CARD_ADDED) {
		g_warning ("Expected change to be E_BOOK_CHANGE_CARD_ADDED, but didn't get it.\n");
		e_book_free_change_list (changes);
		g_object_unref (contact); 
		g_object_unref (book); 
		return;
	}	

	/* Remember pertinent information */
	str = e_contact_get_const (change->contact, E_CONTACT_UID);
	if (!str) {
		g_warning ("Could not get new contact's ID");
	} else {
		/* Remember this contact id */
		new_contact_id = g_strdup (str);

		d(g_print ("Added new contact with ID is '%s'\n", new_contact_id));
	}

	e_book_free_change_list (changes);
	g_object_unref (contact); 
	g_object_unref (book); 
}

static void
post_test (void)
{
	EBook    *book;
	GError   *error = NULL;
	gboolean  success;

	if (!new_contact_id) {
		g_warning ("Contact ID was NULL, it looks like "
			   "the test was not set up correctly");
		return;
	}

	/* Remove the added test contact from the addressbook here */
	book = e_book_new_system_addressbook (NULL);
	g_return_if_fail (book != NULL);

	success = e_book_open (book, FALSE, &error);
	if (!success) {
		g_warning ("Could not open address book, %s", 
			   error ? error->message : "no error given");
		g_clear_error (&error);
		g_object_unref (book); 
		return;
	}

	success = e_book_remove_contact (book, new_contact_id, &error);
	if (!success) {
		g_warning ("Could not remove new contact with ID:'%s', %s", 
			   new_contact_id,
			   error ? error->message : "no error given");
		g_clear_error (&error);
	} else {
		d(g_print ("Removed new contact with ID is '%s'\n", new_contact_id));
	}

	g_object_unref (book); 

	new_contact_id = NULL;
}

START_TEST (test_addressbook_list)
{
	DBusGProxy  *proxy;
	gchar      **ids, **p;
	gint         count;
	gboolean     found;
	gboolean     success;

	/* Test the list call here */
	/* 1. Check that the command run successfully */
	/* 2. Check that our contact is among the listed */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_list (proxy, &ids, NULL);
	fail_if (success == FALSE);
	fail_if (ids == NULL);

	for (p = ids, found = FALSE, count = 0; 
	     *p && !found; 
	     p++, count++) {
		if (strcmp (*p, new_contact_id) == 0) {
			found = TRUE;
		}
	}
	
	d(g_print ("Address book has %d contacts and our new contact was %sfound\n", 
		   count, found ? "" : "not "));

	fail_if (found == FALSE);
}
END_TEST

START_TEST (test_addressbook_owner)
{
	DBusGProxy *proxy;
	gchar      *contact_id;
	gboolean    success;

	/* Test the owner call here */
	/* 1. Check that the command run successfully */
	/* 2. Unsure if we can really do any data checking here? */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_owner (proxy, &contact_id, NULL);
	fail_if (success == FALSE);
	fail_if (contact_id == NULL);

	d(g_print ("Own contact is '%s'\n", contact_id));
}
END_TEST

START_TEST (test_addressbook_find_by_name)
{
	DBusGProxy  *proxy;
	gchar      **ids, **p;
	gint         count;
	gboolean     found;
	gboolean     success;

	/* Test the owner call here */
	/* 1. Check that the command run successfully */
	/* 2. Check that the ID we got back was the one we added above */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_find_by_name (proxy, CONTACT_FULL_NAME, &ids, NULL);
	fail_if (success == FALSE);
	fail_if (ids == NULL);

	for (p = ids, found = FALSE, count = 0; 
	     *p && !found; 
	     p++, count++) {
		if (strcmp (*p, new_contact_id) == 0) {
			found = TRUE;
		}
	}
	
	d(g_print ("Address book found %d contacts matching and our new contact was %sfound\n", 
		   count, found ? "" : "not "));

	fail_if (found == FALSE);
}
END_TEST

START_TEST (test_addressbook_get_name)
{
	DBusGProxy *proxy;
	gchar      *given_name;
	gchar      *family_name;
	gchar      *full_name;
	gboolean    success;

	/* Test the owner call here */
	/* 1. Check that the command run successfully */
	/* 2. Check that the name is correctly retrieved */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_get_name (proxy, new_contact_id, 
							      &given_name, 
							      &family_name, 
							      &full_name,
							      NULL);
	fail_if (success == FALSE);
	fail_if (strcmp (given_name, CONTACT_GIVEN_NAME) != 0);
	fail_if (strcmp (family_name, CONTACT_FAMILY_NAME) != 0);
	fail_if (strcmp (full_name, CONTACT_FULL_NAME) != 0);

	d(g_print ("New contact's names are, given:'%s', family:'%s', full:'%s'\n", 
		   given_name, family_name, full_name));

}
END_TEST

START_TEST (test_addressbook_get_emails)
{
	DBusGProxy  *proxy;
	gchar      **addresses, **p;
	gint         count, found;
	gboolean     success;

	/* Test the owner call here */
	/* 1. Check that the command run successfully */
	/* 2. Check that all the emails (we should set more than one) are
	 *    retrieved correctly */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_get_emails (proxy, new_contact_id, &addresses, NULL);
	fail_if (success == FALSE);
	fail_if (addresses == NULL);

	for (p = addresses, found = 0, count = 0; 
	     *p && found < 3; 
	     p++, count++) {
		if (strcmp (*p, CONTACT_EMAIL_1) == 0 ||
		    strcmp (*p, CONTACT_EMAIL_2) == 0 ||
		    strcmp (*p, CONTACT_EMAIL_3) == 0) {
			found++;
		}
	}
	
	d(g_print ("Address book found %d email addresses\n", found));

	fail_if (found < 3);
}
END_TEST

START_TEST (test_addressbook_get_vcard30)
{
	DBusGProxy *proxy;
	gchar      *vcard;
	gboolean    success;

	/* Test the owner call here */
	/* 1. Check that the command run successfully */
	/* 2. Check that the vcard looks good */

	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	success = org_freedesktop_dapi_address_book_get_vcard30 (proxy, new_contact_id, &vcard, NULL);
	fail_if (success == FALSE);
	fail_if (vcard == NULL);
	fail_if (strstr (vcard, CONTACT_FULL_NAME) == NULL);

	d(g_print ("VCard for contact ID:'%s' is\n%s\n", new_contact_id, vcard));

	/* Expected failures */
	success = org_freedesktop_dapi_address_book_get_vcard30 (proxy, NULL, &vcard, NULL);
	fail_if (success == TRUE);
}
END_TEST

Suite *
tests_create_addressbook_test_suite (void)
{
	Suite *suite;
	TCase *t_case;

	suite = suite_create ("Addressbook");

	t_case = tcase_create ("Api");
	tcase_add_checked_fixture (t_case, pre_test, post_test);
	tcase_add_test (t_case, test_addressbook_list);
	tcase_add_test (t_case, test_addressbook_owner);
	tcase_add_test (t_case, test_addressbook_find_by_name);
	tcase_add_test (t_case, test_addressbook_get_name);
	tcase_add_test (t_case, test_addressbook_get_emails);
	tcase_add_test (t_case, test_addressbook_get_vcard30);

	suite_add_tcase (suite, t_case);

	return suite;
}
