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
 * Author: Martyn Russell <martyn@imendio.com>
 */
#include <stdlib.h>

#include <glib.h>
#include <libebook/e-book.h>

/* Abstract layer */
typedef void (*NamesCallback)  (const gchar *id,
				const gchar *given_name,
				const gchar *family_name,
				const gchar *full_name,
				gpointer     user_data); 
typedef void (*EmailsCallback) (const gchar *id,
				GList       *addresses,
				gpointer     user_data); 


static void     address_book_get_names_cb           (EBook           *book,
						     EBookStatus      status,
						     EContact        *contact,
						     gpointer         closure);
static void     address_book_get_email_addresses_cb (EBook           *book,
						     EBookStatus      status,
						     EContact        *contact,
						     gpointer         closure);
static void     address_book_loaded_cb              (EBook           *book,
						     EBookStatus      status,
						     gpointer         closure);
static gboolean address_book_get_names              (const gchar     *id,
						     NamesCallback    callback,
						     gpointer         user_data);
static gboolean address_book_get_emails             (const gchar     *id,
						     EmailsCallback   callback,
						     gpointer         user_data);
static gboolean address_book_get_owner              (gchar          **id);


/* My functions */
static void     should_quit                         (void);
static void     get_names_cb                        (const gchar     *id,
						     const gchar     *given_name,
						     const gchar     *family_name,
						     const gchar     *full_name,
						     gpointer         user_data);
static void     get_email_addresses_cb              (const gchar     *id,
						     GList           *addresses,
						     gpointer         user_data);

static GMainLoop *main_loop = NULL;
static gboolean   have_names = FALSE;
static gboolean   have_email_addresses = FALSE;

static void
address_book_get_names_cb (EBook       *book, 
			   EBookStatus  status, 
			   EContact    *contact, 
			   gpointer     closure)
{
	const gchar   *id;
	NamesCallback  callback;
	gpointer       user_data;

	const gchar   *given_name = NULL;
	const gchar   *family_name = NULL;
	const gchar   *full_name = NULL;

/* 	g_print ("EContact is %p\n", contact);  */

	/* FIXME: This should NOT fail */
	g_return_if_fail (contact != NULL);
	g_return_if_fail (E_IS_CONTACT (contact));

	if (status != E_BOOK_ERROR_OK) {
		g_warning ("Could not get contact, status is %d", status);
		return;
	}

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	g_print ("Getting address book names (given, family, full) "
		 "for user ID:'%s'...", id);
	
	given_name = e_contact_get_const (contact, E_CONTACT_GIVEN_NAME);
	family_name = e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);
	full_name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);

	(callback) (id, given_name, family_name, full_name, user_data);

 	g_object_unref (book); 
}

static void
address_book_get_email_addresses_cb (EBook       *book, 
				     EBookStatus  status, 
				     EContact    *contact, 
				     gpointer     closure)
{
	const gchar    *id;
	EmailsCallback  callback;
	gpointer        user_data;
	
	GList          *addresses = NULL;
	
/* 	g_print ("EContact is %p\n", contact);  */

	/* FIXME: This should NOT fail */
	g_return_if_fail (contact != NULL);
	g_return_if_fail (E_IS_CONTACT (contact));

	if (status != E_BOOK_ERROR_OK) {
		g_warning ("Could not get all emails, status is %d", status);
		return;
	}

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	g_print ("Getting address book email addresses for user ID:'%s'...", id);
	
	addresses = e_contact_get (contact, E_CONTACT_EMAIL);

	(callback) (id, addresses, user_data);

	g_list_foreach (addresses, (GFunc) g_free, NULL);
	g_list_free (addresses);

 	g_object_unref (book); 
}

static void
address_book_loaded_cb (EBook       *book, 
			EBookStatus  status, 
			gpointer     closure)
{
	EBookContactCallback  func;
	const gchar          *id;
	guint                 result;

	id = g_object_get_data (G_OBJECT (book), "id");
	func = g_object_get_data (G_OBJECT (book), "func");

	if (status != E_BOOK_ERROR_OK) {
		g_warning ("Could not load book, status is %d", status);
		return;
	}

	result = e_book_async_get_contact (book, id, 
					   (EBookContactCallback) func, 
					   NULL);

	if (!result) {
		g_warning ("Could not get contact:'%s', "
			   "e_book_async_get_contact() failed", 
			   id);
		return;
	}
}

static gboolean
address_book_get_names (const gchar   *id,
			NamesCallback  callback,
			gpointer       user_data)
{
	EBook *book;

	g_return_val_if_fail (id != NULL, FALSE);
	g_return_val_if_fail (callback != NULL, FALSE);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		g_warning ("Couldn't get ebook");
		return FALSE;
	}

	g_object_set_data_full (G_OBJECT (book), "id", g_strdup (id), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
 	g_object_set_data (G_OBJECT (book), "func", address_book_get_names_cb); 
	
 	e_book_async_open (book, FALSE, (EBookCallback) address_book_loaded_cb, NULL); 

	return TRUE;
}

static gboolean
address_book_get_emails (const gchar    *id,
			 EmailsCallback  callback,
			 gpointer        user_data)
{
	EBook *book;

	g_return_val_if_fail (id != NULL, FALSE);
	g_return_val_if_fail (callback != NULL, FALSE);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		g_warning ("Couldn't get ebook");
		return FALSE;
	}

	g_object_set_data_full (G_OBJECT (book), "id", g_strdup (id), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
	g_object_set_data (G_OBJECT (book), "func", address_book_get_email_addresses_cb);
	
 	e_book_async_open (book, FALSE, 
			   (EBookCallback) address_book_loaded_cb, NULL); 

	return TRUE;
}

static gboolean
address_book_get_owner (gchar **id)
{
	EBook    *book;
	EContact *contact;
	GError   *error = NULL;

	g_return_val_if_fail (id != NULL, FALSE);

	if (!e_book_get_self (&contact, &book, &error)) {
		g_warning ("Could not get own contact details, %s", 
			   error ? error->message : "");
		g_clear_error (&error);

		return FALSE;
	}

	*id = e_contact_get (contact, E_CONTACT_UID);

	g_object_unref (contact);
	g_object_unref (book);

	return TRUE;
}

/*
 * My callbacks.
 */

static void 
should_quit (void)
{
	if (have_names && have_email_addresses) {
		g_main_loop_quit (main_loop);
	}
}

static void
get_names_cb (const gchar *id,
	      const gchar *given_name,
	      const gchar *family_name,
	      const gchar *full_name,
	      gpointer     user_data)
{
	g_print ("<--- Found given name:'%s', family name:'%s', full name:'%s'\n", 
		 given_name, family_name, full_name);

	should_quit ();
}

static void
get_email_addresses_cb (const gchar *id,
			 GList       *addresses,
			 gpointer     user_data)
{
	g_print ("<--- Found %d email addresses\n", g_list_length (addresses));

	should_quit ();
}

int
main (int argc, char *argv[])
{
	gchar *id = NULL;

	address_book_get_owner (&id);
	if (!id) {
		return EXIT_FAILURE;
	}

	g_print ("Address book owner has ID:'%s'\n", id);

	address_book_get_emails (id, get_email_addresses_cb, NULL);
	address_book_get_names (id, get_names_cb, NULL);

	g_free (id);

	main_loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (main_loop);

	g_print ("Quitting...\n");

	return EXIT_SUCCESS;
}
