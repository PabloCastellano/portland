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
#include <glib/gi18n.h> 

#include <dapi-common.h>

#include "dapi-command-bindings.h"

#define DAPI_DBUS_SERVICE   "org.freedesktop.dapi"
#define DAPI_DBUS_PATH      "/org/freedesktop/dapi"
#define DAPI_DBUS_INTERFACE "org.freedesktop.dapi"

static gchar *     test_capabilities_to_str         (gint          capability);
static gboolean    test_show_capabilities           (DBusGProxy   *proxy);
static gboolean    test_open_url                    (DBusGProxy   *proxy,
						     const gchar  *url,
						     GValue       *windowinfo);
static gboolean    test_execute_url                 (DBusGProxy   *proxy,
						     const gchar  *url,
						     GValue       *windowinfo);
static gboolean    test_button_order                (DBusGProxy   *proxy);
static gboolean    test_run_as_user                 (DBusGProxy   *proxy,
						     const gchar  *user,
						     const gchar  *command,
						     GValue       *windowinfo);
static gboolean    test_suspend_screen_saving       (DBusGProxy   *proxy,
						     gint          seconds);
static gboolean    test_mailto                      (DBusGProxy   *proxy,
						     const gchar  *subject,
						     const gchar  *body,
						     const gchar  *to,
						     const gchar  *cc,
						     const gchar  *bcc,
						     const gchar **attachments,
						     GValue       *windowinfo);
static void        test_local_file_cb               (DBusGProxy   *proxy,
						     gchar        *filename,
						     GError       *error,
						     gpointer      user_data);
static gboolean    test_local_file                  (DBusGProxy   *proxy,
						     const gchar  *remote_url,
						     const gchar  *local_url,
						     gboolean      allow_download,
						     GValue       *windowinfo);
static void        test_upload_file_cb              (DBusGProxy   *proxy,
						     GError       *error,
						     gpointer      user_data);
static gboolean    test_upload_file                 (DBusGProxy   *proxy,
						     const gchar  *local_url,
						     const gchar  *remote_url,
						     gboolean      remove_local,
						     GValue       *windowinfo);
static gboolean    test_remove_temporary_local_file (DBusGProxy   *proxy,
						     const gchar  *url);
static void        test_address_book_get_list_cb    (DBusGProxy   *proxy,
						     gchar       **ids,
						     GError       *error,
						     gpointer      user_data);
static gboolean    test_address_book_get_list       (DBusGProxy   *proxy);
static void        test_address_book_get_name_cb    (DBusGProxy   *proxy,
						     gchar        *given_name,
						     gchar        *family_name,
						     gchar        *full_name,
						     GError       *error,
						     gpointer      user_data);
static gboolean    test_address_book_get_name       (DBusGProxy   *proxy,
						     const gchar  *id);
static void        test_address_book_get_emails_cb  (DBusGProxy   *proxy,
						     gchar       **email_addresses,
						     GError       *error,
						     gpointer      user_data);
static gboolean    test_address_book_get_emails     (DBusGProxy   *proxy,
						     const gchar  *id);
static gboolean    test_address_book_find_by_name   (DBusGProxy   *proxy,
						     const gchar  *name);
static gboolean    test_address_book_get_owner      (DBusGProxy   *proxy);
static gboolean    test_address_book_get_vcard      (DBusGProxy   *proxy,
						     const gchar  *id);
static void        test_address_book_get_vcard30_cb (DBusGProxy   *proxy,
						     gchar        *vcard,
						     GError       *error,
						     gpointer      user_data);
static DBusGProxy *test_get_dbus_proxy              (void);
static void        test_status_update               (gboolean      complete);

static gboolean      show_capabilities = FALSE;
static const gchar  *open_url = NULL;
static const gchar  *execute_url = NULL;
static gboolean      button_order = FALSE;
static const gchar  *run_user = NULL;
static const gchar  *run_command = NULL;
static gint          suspend_screensaver = 0;
static const gchar  *mail_subject = NULL;
static const gchar  *mail_body = NULL;
static const gchar  *mail_to = NULL;
static const gchar  *mail_cc = NULL;
static const gchar  *mail_bcc = NULL;
static const gchar **mail_attachments = NULL;
static const gchar  *local_url = NULL;
static gboolean      allow_download = FALSE;
static const gchar  *upload_url = NULL;
static gboolean      remove_local = FALSE;
static const gchar  *remote_url = NULL;
static const gchar  *remove_temporary_local_file = NULL;
static gboolean      address_book_get_list = FALSE;
static const gchar  *address_book_get_name = NULL;
static const gchar  *address_book_get_emails = NULL;
static const gchar  *address_book_find_by_name = NULL;
static gboolean      address_book_get_owner = FALSE;
static const gchar  *address_book_get_vcard = NULL;

static GMainLoop    *main_loop = NULL;
static gint          tasks_waiting = 0;

static const GOptionEntry options[] = {
 	{ "show-capabilities", 'c', 
	  0, G_OPTION_ARG_NONE, &show_capabilities,
	  N_("Show a list of capabilities"),
	  NULL },
	{ "open-url", 'o', 
	  0, G_OPTION_ARG_STRING, &open_url,
	  N_("Open a URL (e.g. http://www.google.com/) using the default browser"),
	  NULL },
	{ "execute-url", 'x', 
	  0, G_OPTION_ARG_STRING, &execute_url,
	  N_("Open a URL (e.g. file:///tmp/test.txt) using the default application"),
	  NULL },
	{ "button-order", 'b', 
	  0, G_OPTION_ARG_NONE, &button_order,
	  N_("Retrieve the current button order (e.g. OK/Cancel or Cancel/OK)"),
	  NULL },
	{ "run-as-user", 'u', 
	  0, G_OPTION_ARG_STRING, &run_user,
	  N_("Used with --run-command, user to run as (e.g. root)"),
	  NULL },
	{ "run-command", 'r', 
	  0, G_OPTION_ARG_STRING, &run_command,
	  N_("Used with --run-as-user, command to run (e.g. /usr/bin/synaptic)"),
	  NULL },
	{ "suspend-screensaver", 's', 
	  0, G_OPTION_ARG_INT, &suspend_screensaver,
	  N_("Suspend the screensaver for the number of seconds specified"),
	  NULL },
	{ "mail-to", 'm', 
	  0, G_OPTION_ARG_STRING, &mail_to,
	  N_("Send a mail to the address specified"),
	  NULL },
	/* Add a bunch of flags for the mailto test */
	{ "mail-subject", 0, 
	  0, G_OPTION_ARG_STRING, &mail_subject,
	  N_("Used with --mail-to, subject of the email"),
	  NULL },
	{ "mail-body", 0, 
	  0, G_OPTION_ARG_STRING, &mail_body,
	  N_("Used with --mail-to, body of the email"),
	  NULL },
	{ "mail-cc", 0, 
	  0, G_OPTION_ARG_STRING, &mail_cc,
	  N_("Used with --mail-to, cc addresses to send to"),
	  NULL },
	{ "mail-bcc", 0, 
	  0, G_OPTION_ARG_STRING, &mail_bcc,
	  N_("Used with --mail-to, bcc addresses to send to"),
	  NULL },
	{ "mail-attachment", 0, 
	  0, G_OPTION_ARG_STRING_ARRAY, &mail_attachments,
	  N_("Used with --mail-to, attachment to send (can be used multiple times)"),
	  NULL },
	{ "local-url", 0, 
	  0, G_OPTION_ARG_STRING, &local_url,
	  N_("Used with --remote-url and --allow-download, Copy remote url to local file"),
	  NULL },
	{ "upload-url", 0,
	  0, G_OPTION_ARG_STRING, &upload_url,
	  N_("Used with --remote-url and --remove-local, Copy local file back to remote url"),
	  NULL },
	{ "remote-url", 0, 
	  0, G_OPTION_ARG_STRING, &remote_url,
	  N_("Used with --local-url and --upload-url, this is the remote url to download or upload"),
	  NULL },
	{ "allow-download", 0, 
	  0, G_OPTION_ARG_NONE, &allow_download,
	  N_("Used with --local-url, allow the remote url to be downloaded (optional, default is FALSE)"),
	  NULL },
	{ "remove-temporary-local-file", 0, 
	  0, G_OPTION_ARG_STRING, &remove_temporary_local_file,
	  N_("Used with --local-url, this removes a temporary file created by --local-url"),
	  NULL },
	{ "remove-local", 0, 
	  0, G_OPTION_ARG_NONE, &remove_local,
	  N_("Used with --upload-url, remove local temporary file after uploaded (optional, default is FALSE)"),
	  NULL },
	{ "address-book-get-list", 'l', 
	  0, G_OPTION_ARG_NONE, &address_book_get_list,
	  N_("Retrieve address book user IDs"),
	  NULL },
	{ "address-book-get-names", 'n', 
	  0, G_OPTION_ARG_STRING, &address_book_get_name,
	  N_("Retrieve names associated with a user ID"),
	  NULL },
	{ "address-book-get-emails", 'e', 
	  0, G_OPTION_ARG_STRING, &address_book_get_emails,
	  N_("Retrieve emails addresses associated with a user ID"),
	  NULL },
	{ "address-book-find-by-name", 'f', 
	  0, G_OPTION_ARG_STRING, &address_book_find_by_name,
	  N_("Find a contact's user ID by their real name."),
	  NULL },
	{ "address-book-get-owner", 'w', 
	  0, G_OPTION_ARG_NONE, &address_book_get_owner,
	  N_("Retrieve address book owner's user ID"),
	  NULL },
	{ "address-book-get-vcard", 'v', 
	  0, G_OPTION_ARG_STRING, &address_book_get_vcard,
	  N_("Retrieve the vcard associated with a user ID"),
	  NULL },
	{ NULL }
};	

static gchar *
test_capabilities_to_str (gint capability) 
{
	GString *s;

	s = g_string_new ("");

	if (capability & DAPI_CAP_OPENURL) 
		s = g_string_append (s, "DAPI_CAP_OPENURL | ");
	if (capability & DAPI_CAP_EXECUTEURL)
		s = g_string_append (s, "DAPI_CAP_EXECUTEURL | ");
	if (capability & DAPI_CAP_BUTTONORDER) 
		s = g_string_append (s, "DAPI_CAP_BUTTONORDER | ");
	if (capability & DAPI_CAP_RUNASUSER) 
		s = g_string_append (s, "DAPI_CAP_RUNASUSER | ");
	if (capability & DAPI_CAP_SUSPENDSCREENSAVING) 
		s = g_string_append (s, "DAPI_CAP_SUSPENDSCREENSAVING | ");
	if (capability & DAPI_CAP_MAILTO) 
		s = g_string_append (s, "DAPI_CAP_MAILTO | ");
	if (capability & DAPI_CAP_LOCALFILE) 
		s = g_string_append (s, "DAPI_CAP_LOCALFILE | ");
	if (capability & DAPI_CAP_UPLOADFILE) 
		s = g_string_append (s, "DAPI_CAP_UPLOADFILE | ");
	if (capability & DAPI_CAP_REMOVETEMPORARYLOCALFILE) 
		s = g_string_append (s, "DAPI_CAP_REMOVETEMPORARYLOCALFILE | ");
	if (capability & DAPI_CAP_ADDRESSBOOKLIST) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKLIST | ");
	if (capability & DAPI_CAP_ADDRESSBOOKGETNAME) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKGETNAME | ");
	if (capability & DAPI_CAP_ADDRESSBOOKGETEMAILS) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKGETEMAILS | ");
	if (capability & DAPI_CAP_ADDRESSBOOKFINDBYNAME) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKFINDBYNAME | ");
	if (capability & DAPI_CAP_ADDRESSBOOKOWNER) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKOWNER | ");
	if (capability & DAPI_CAP_ADDRESSBOOKGETVCARD30) 
		s = g_string_append (s, "DAPI_CAP_ADDRESSBOOKGETVCARD30 | ");

	s->len -= 3;
	s->str[s->len] = '\0';

	return g_string_free (s, FALSE);	
}

static gboolean
test_show_capabilities (DBusGProxy *proxy)
{
	gint    capabilities;
	gchar  *str, **strv;
	GError *error = NULL;

	if (!org_freedesktop_dapi_capabilities (proxy, &capabilities, &error)) {
		g_printerr ("Could not get capabilities, error: %s\n", 
			    error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	if (capabilities < 1) {
		g_print ("No capabilities returned\n");
		return TRUE;
	}

	g_print ("Capabilities (%d):\n", capabilities);
	
	/* Use for an int not an array of ints */
	str = test_capabilities_to_str (capabilities);
	strv = g_strsplit (str, "|", -1);
	g_free (str);

	str = g_strjoinv ("\n\t --->", strv);
	g_strfreev (strv);

	g_print ("\t ---> %s\n", str);
	g_free (str);

	return TRUE;
}

static gboolean
test_open_url (DBusGProxy  *proxy,
	       const gchar *url,
	       GValue      *windowinfo)
{
	GError *error = NULL;

	if (!org_freedesktop_dapi_open_url (proxy, url, windowinfo, &error)) {
		g_printerr ("Could not open URL:'%s', error: %s\n", 
			    url, error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Opened URL:'%s'\n", url);

	return TRUE;
}

static gboolean
test_execute_url (DBusGProxy  *proxy,
		  const gchar *url,
		  GValue      *windowinfo)
{
	GError *error = NULL;

	if (!org_freedesktop_dapi_execute_url (proxy, url, windowinfo, &error)) {
		g_printerr ("Could not execute URL:'%s', error: %s\n", 
			    url, error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Executed URL:'%s'\n", url);
	
	return TRUE;
}

static gboolean 
test_button_order (DBusGProxy *proxy)
{
	GError      *error = NULL;
	const gchar *str = "Unknown";
	gint         button_order;

	if (!org_freedesktop_dapi_button_order (proxy, &button_order, &error)) {
		g_printerr ("Could not get button order, error: %s\n", 
			    error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	if (button_order == 1) {
		str = "Cancel/OK";
	} else if (button_order == 2) {
		str = "OK/Cancel";
	}

	g_print ("Button order is %s\n", str);

	return TRUE;
}

static gboolean
test_run_as_user (DBusGProxy  *proxy,
		  const gchar *user,
		  const gchar *command,
		  GValue      *windowinfo)
{
	GError *error = NULL;

	if (!org_freedesktop_dapi_run_as_user (proxy, user, command, windowinfo, &error)) {
		g_printerr ("Could not run command:'%s' as user:'%s', error: %s\n", 
			    command, user, error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Ran command:'%s' as user:'%s'\n", command, user);

	return TRUE;
}

static gboolean
test_suspend_screen_saving (DBusGProxy *proxy,
			    gint        seconds)
{
	GError *error = NULL;
	guint   client_id = 1;

	if (!org_freedesktop_dapi_suspend_screen_saving (proxy, client_id, TRUE, &error)) {
		g_printerr ("Could not suspend screen saving, error: %s\n", 
			    error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Suspended screen saving for %d seconds.\n", seconds);

	g_usleep (G_USEC_PER_SEC * seconds);
	
	if (!org_freedesktop_dapi_suspend_screen_saving (proxy, client_id, FALSE, &error)) {
		g_printerr ("Could not resumed screen saving, error: %s\n", 
			    error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	return TRUE;
}

static gboolean
test_mailto (DBusGProxy   *proxy,
	     const gchar  *subject,
	     const gchar  *body,
	     const gchar  *to,
	     const gchar  *cc,
	     const gchar  *bcc,
	     const gchar **attachments,
	     GValue       *windowinfo)
{
	GError       *error = NULL;
	const gchar **p;
	const gchar  *nullp[] = { 0, };

  	if (attachments) {  
  		p = attachments;  
  	} else {  
  		p = nullp;
  	}  

	if (!org_freedesktop_dapi_mail_to (proxy, subject, body, to, cc, bcc, p, windowinfo, &error)) {
		g_printerr ("Could not create new mail to:'%s', error: %s\n", 
			    to, error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Created new mail to:'%s'\n", to);

	return TRUE;
}

static void
test_local_file_cb (DBusGProxy *proxy,
		    gchar      *filename, 
		    GError     *error,
		    gpointer    user_data)
{
	gchar *remote_url;

	remote_url = user_data;
	
	if (error) {
		g_print ("Could not create local file from url:'%s', error: %s\n",
			 remote_url, error->message);
	} else {
		g_print ("Created local file from url:'%s' as:'%s'\n", 
			 remote_url, filename);
	}

	g_free (remote_url);

	test_status_update (TRUE);
}
		    
static gboolean
test_local_file (DBusGProxy  *proxy,
		 const gchar *remote_url,
		 const gchar *local_url,
		 gboolean     allow_download,
		 GValue      *windowinfo)
{
	if (!org_freedesktop_dapi_local_file_async (proxy, 
						    remote_url, 
						    local_url, 
						    allow_download, 
						    windowinfo, 
						    test_local_file_cb,
						    g_strdup (remote_url))) {
		g_printerr ("Could not create local file:'%s' from remote file:'%s', no error given\n", 
			    local_url, remote_url);
		return FALSE;
	}

	test_status_update (FALSE);

	return TRUE;
}

static void
test_upload_file_cb (DBusGProxy *proxy,
		     GError     *error,
		     gpointer    user_data)
{
	gchar *local_url;

	local_url = user_data;
	
	if (error) {
		g_print ("Could not upload local file:'%s', error: %s\n",
			 local_url, error->message);
	} else {
		g_print ("Uploaded local file:'%s'\n", 
			 local_url);
	}

	g_free (local_url);

	test_status_update (TRUE);
}

static gboolean
test_upload_file (DBusGProxy  *proxy,
		  const gchar *local_url,
		  const gchar *remote_url,
		  gboolean     remove_local,
		  GValue      *windowinfo)
{
	if (!org_freedesktop_dapi_upload_file_async (proxy, 
						     remote_url, 
						     local_url, 
						     allow_download, 
						     windowinfo, 
						     test_upload_file_cb,
						     g_strdup (local_url))) {
		g_printerr ("Could not upload local file:'%s' to remote file:'%s', no error given\n", 
			    local_url, remote_url);
		return FALSE;
	}

	test_status_update (FALSE);

	return TRUE;
}

static gboolean
test_remove_temporary_local_file (DBusGProxy  *proxy,
				  const gchar *local)
{
	GError *error = NULL;

	if (!org_freedesktop_dapi_remove_temporary_local_file (proxy, local, &error)) {
		g_printerr ("Could not remove temporary local file:'%s', error: %s\n", 
			    local, error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Removed temporary local file:'%s'\n", local);

	return TRUE;
}

static void
test_address_book_get_list_cb (DBusGProxy   *proxy,
			       gchar       **ids,
			       GError       *error,
			       gpointer      user_data)
{
	if (error) {
		g_print ("Could not get address book list, error: %s\n",
			 error->message);
	} else {
		gchar **p;
		
		g_print ("Address book list\n");
		
		for (p = ids; *p; p++) {
			g_print ("\t\"%s\"\n", *p);
		}
	}
	
	test_status_update (TRUE);
}

static gboolean 
test_address_book_get_list (DBusGProxy *proxy)
{
	if (!org_freedesktop_dapi_address_book_list_async (proxy, 
							   test_address_book_get_list_cb,
							   NULL)) {
		g_printerr ("Could not get address book list, no error given\n");
		return FALSE;
	}
	
	test_status_update (FALSE);
	
	return TRUE;
}

static void
test_address_book_get_name_cb (DBusGProxy   *proxy,
			       gchar        *given_name,
			       gchar        *family_name,
			       gchar        *full_name,
			       GError       *error,
			       gpointer      user_data)
{
	gchar *id;

	id = user_data;

	if (error) {
		g_print ("Could not get address book names for id:'%s', error: %s\n",
			 id, error->message);
	} else {
		if (!given_name && !family_name && !full_name) {
			g_print ("No names are associated with id:'%s'\n", id);
		} else {
			g_print ("Names for id:'%s'\n", id);

			if (given_name) {
				g_print ("\tGiven name:'%s'\n", given_name);
			}
			
			if (family_name) {
				g_print ("\tFamily name:'%s'\n", family_name);
			}
			
			if (full_name) {
				g_print ("\tFull name:'%s'\n", full_name);
			}
		}
	}

	g_free (id);
	
	test_status_update (TRUE);
}

static gboolean 
test_address_book_get_name (DBusGProxy  *proxy,
			    const gchar *id)
{
	if (!org_freedesktop_dapi_address_book_get_name_async (proxy, 
							       id,
							       test_address_book_get_name_cb,
							       g_strdup (id))) {
		g_printerr ("Could not get address book names "
			    "for id:'%s', no error given\n", 
			    id);
		return FALSE;
	}
	
	test_status_update (FALSE);
	
	return TRUE;
}

static void
test_address_book_get_emails_cb (DBusGProxy   *proxy,
				 gchar       **email_addresses,
				 GError       *error,
				 gpointer      user_data)
{
	gchar *id;

	id = user_data;

	if (error) {
		g_print ("Could not get address book email addresses "
			 "for contact:'%s', error: %s\n",
			 id, error->message);
	} else {
		gchar **p;
		
		g_print ("Email addresses:\n");
		
		for (p = email_addresses; *p; p++) {
			g_print ("\t\"%s\"\n", *p);
		}
	}

	g_free (id);
	
	test_status_update (TRUE);
}

static gboolean 
test_address_book_get_emails (DBusGProxy  *proxy,
			      const gchar *id)
{
	if (!org_freedesktop_dapi_address_book_get_emails_async (proxy, 
								 id,
								 test_address_book_get_emails_cb,
								 g_strdup (id))) {
		g_printerr ("Could not get address book email addresses "
			    "for id:'%s', no error given\n", 
			    id);
		return FALSE;
	}
	
	test_status_update (FALSE);

	return TRUE;
}

static void
test_address_book_find_by_name_cb (DBusGProxy   *proxy,
				   gchar       **ids,
				   GError       *error,
				   gpointer      user_data)
{
	gchar *name;

	name = user_data;

	if (error) {
		g_print ("Could not find address book contacts "
			 "by name:'%s', error: %s\n",
			 name, error->message);
	} else {
		gchar **p;
		
		g_print ("Contacts:\n");
		
		for (p = ids; *p; p++) {
			g_print ("\t\"%s\"\n", *p);
		}
	}

	g_free (name);
	
	test_status_update (TRUE);
}

static gboolean 
test_address_book_find_by_name (DBusGProxy  *proxy,
				const gchar *name)
{
	if (!org_freedesktop_dapi_address_book_find_by_name_async (proxy, 
								   name,
								   test_address_book_find_by_name_cb,
								   g_strdup (name))) {
		g_printerr ("Could not find address book contacts "
			    "by name:'%s', no error given\n", 
			    name);
		return FALSE;
	}
	
	test_status_update (FALSE);

	return TRUE;
}

static gboolean 
test_address_book_get_owner (DBusGProxy *proxy)
{
	GError *error = NULL;
	gchar  *contact_id;

	if (!org_freedesktop_dapi_address_book_owner (proxy, &contact_id, &error)) {
		g_printerr ("Could not get address book owner, error: %s\n", 
			    error ? error->message : "none");
		g_clear_error (&error);
		return FALSE;
	}

	g_print ("Address book owner has contact id:'%s'\n", contact_id);

	return TRUE;
}

static void
test_address_book_get_vcard30_cb (DBusGProxy  *proxy,
				  gchar       *vcard,
				  GError      *error,
				  gpointer     user_data)
{
	gchar *id;

	id = user_data;

	if (error) {
		g_print ("Could not get vcard for contact "
			 "by id:'%s', error: %s\n",
			 id, error->message);
	} else {
		g_print ("VCard:\n%s\n", vcard);
	}

	g_free (id);
	
	test_status_update (TRUE);
}

static gboolean 
test_address_book_get_vcard (DBusGProxy  *proxy,
			     const gchar *id)
{
	if (!org_freedesktop_dapi_address_book_get_vcard30_async (proxy, 
								  id,
								  test_address_book_get_vcard30_cb,
								  g_strdup (id))) {
		g_printerr ("Could not get vcard for contact "
			    "by id:'%s', no error given\n", 
			    id);
		return FALSE;
	}
	
	test_status_update (FALSE);

	return TRUE;
}

static DBusGProxy *
test_get_dbus_proxy (void)
{
	DBusGConnection   *connection;
	static DBusGProxy *proxy = NULL;
	GError            *error = NULL;
  
	if (proxy) {
		return proxy;
	}

	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	if (connection == NULL) {
		g_warning ("Failed to open connection to session bus: %s\n",
			   error->message);
		g_clear_error (&error);
		return NULL;
	}

	proxy = dbus_g_proxy_new_for_name (connection,
					   DAPI_DBUS_SERVICE,
					   DAPI_DBUS_PATH,
					   DAPI_DBUS_INTERFACE);

	return proxy;
}

static void
test_status_update (gboolean complete)
{
	if (complete) {
		tasks_waiting--;

		if (tasks_waiting < 1) {
			g_main_loop_quit (main_loop);
		}
	} else {
		tasks_waiting++;
	}
}

int
main (int argc, char *argv[])
{
	GOptionContext *context;
	DBusGProxy     *proxy;
	GValue          windowinfo = {0, };
	gboolean        success = TRUE;

	g_type_init ();

	context = g_option_context_new ("- Test DAPI Daemon");
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	if ((!show_capabilities && !open_url &&  !execute_url &&
	     !mail_to && !suspend_screensaver && !run_user && 
	     !run_command && !button_order && 
	     !address_book_get_list && !address_book_get_emails && 
	     !address_book_get_name && !address_book_find_by_name && 
	     !address_book_get_owner && !address_book_get_vcard && 
	     !local_url && !remote_url && !allow_download && 
	     !upload_url && !remove_temporary_local_file) ||
	    ((!run_user && run_command) || (!run_command && run_user)) || 
	    ((!local_url && remote_url && !upload_url) || 
	     (!remote_url && local_url && !upload_url)) ||
	    ((!upload_url && remote_url && !local_url) || 
	     (!remote_url && upload_url && !local_url))) {
		g_printerr ("For usage, try %s --help\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	/* Set up DBus connection */
	proxy = test_get_dbus_proxy ();
	if (!proxy) {
		return EXIT_FAILURE;
	}

	/* Set up empty window info, we have to do this because DBus
	 * doesn't NULL check G_TYPE_STRV or variants and SEGFAULTS
	 */
	g_value_init (&windowinfo, G_TYPE_INT);

	/* Run arguments */ 
	if (success && show_capabilities && 
	    !test_show_capabilities (proxy)) {
		success = FALSE;
	}

	if (success && open_url && 
	    !test_open_url (proxy, open_url, &windowinfo)) {
		success = FALSE;
	}

	if (success && execute_url && 
	    !test_execute_url (proxy, execute_url, &windowinfo)) {
		success = FALSE;
	}

	if (success && button_order && 
	    !test_button_order (proxy)) {
		success = FALSE;
	}

	if (success && run_user && run_command && 
	    !test_run_as_user (proxy, run_user, run_command, &windowinfo)) {
		success = FALSE;
	}

	if (success && suspend_screensaver &&
	    !test_suspend_screen_saving (proxy, suspend_screensaver)) {
		success = FALSE;
	}

	if (success && mail_to && 
	    !test_mailto (proxy,
			  mail_subject, 
			  mail_body, 
			  mail_to, 
			  mail_cc, 
			  mail_bcc,
			  mail_attachments,
			  NULL)) {
		success = FALSE;
	}

	if (success && local_url && remote_url &&
	    !test_local_file (proxy, remote_url, local_url, allow_download, &windowinfo)) {
		success = FALSE;
	}

	if (success && upload_url && remote_url &&
	    !test_upload_file (proxy, upload_url, remote_url, remove_local, &windowinfo)) {
		success = FALSE;
	}

	if (success && remove_temporary_local_file && 
	    !test_remove_temporary_local_file (proxy, remove_temporary_local_file)) {
		success = FALSE;
	}

	if (success && address_book_get_list && 
	    !test_address_book_get_list (proxy)) {
		success = FALSE;
	}

	if (success && address_book_get_name && 
	    !test_address_book_get_name (proxy, address_book_get_name)) {
		success = FALSE;
	}

	if (success && address_book_get_emails && 
	    !test_address_book_get_emails (proxy, address_book_get_emails)) {
		success = FALSE;
	}

	if (success && address_book_find_by_name && 
	    !test_address_book_find_by_name (proxy, address_book_find_by_name)) {
		success = FALSE;
	}

	if (success && address_book_get_owner && 
	    !test_address_book_get_owner (proxy)) {
		success = FALSE;
	}

	if (success && address_book_get_vcard && 
	    !test_address_book_get_vcard (proxy, address_book_get_vcard)) {
		success = FALSE;
	}

	/* If we have async functions still to be completed, wait in a
	 * loop for them to be called back.
	 */
	if (tasks_waiting > 0) {
		main_loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (main_loop);

		g_main_loop_unref (main_loop);
	}

	g_object_unref (proxy);
	g_value_unset (&windowinfo);

	return success  ? EXIT_SUCCESS : EXIT_FAILURE;
}
