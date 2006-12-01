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

#include <config.h>

#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gconf/gconf-client.h> 
#include <libebook/e-book.h>

#include <dapi-common.h>

#include "dapi-debug.h"
#include "dapi-gnome.h"

#define DEBUG_DOMAIN "Gnome"

#define DAPI_GNOME_ERROR_DOMAIN "DAPI-Gnome"

#define GCONF_URL_HANDLERS_PATH "/desktop/gnome/url-handlers" 

#define gnome_return_val_if_not_capable(expr,val)                                \
	G_STMT_START {							         \
		if (dapi_gnome_has_capability(expr)) { } else { return (val); }; \
	} G_STMT_END

#define gnome_return_if_not_capable(expr)                                        \
	G_STMT_START {							         \
		if (dapi_gnome_has_capability(expr)) { } else { return; };       \
	} G_STMT_END

typedef enum {
 	GNOME_MODULE_GNOMEVFS    = 1 << 0, 
 	GNOME_MODULE_GTK         = 1 << 1, 
 	GNOME_MODULE_EBOOK       = 1 << 2, 
} GnomeModule;

typedef struct {
	gchar         *filename;
	gboolean       flag;
	GError        *error;
	gconstpointer  callback;
	gpointer       user_data;
} TransferFileData;

static gchar *      gnome_open_url_get_exec            (const char        *scheme);
static const gchar *gnome_vfs_xfer_phase_to_str        (GnomeVFSXferPhase  phase);
static void         gnome_ebook_get_names_cb           (EBook             *book,
							EBookStatus        status,
							GList             *contacts,
							gpointer           closure);
static void         gnome_ebook_get_email_addresses_cb (EBook             *book,
							EBookStatus        status,
							GList             *contacts,
							gpointer           closure);
static void         gnome_ebook_find_by_name_cb        (EBook             *book,
							EBookStatus        status,
							GList             *contacts,
							gpointer           closure);
static void         gnome_ebook_loaded_cb              (EBook             *book,
							EBookStatus        status,
							gpointer           closure);

static gint        capabilities = 0;
static GHashTable *temporary_files = NULL;
static GHashTable *dpms_clients = NULL;

GQuark
dapi_gnome_error_quark (void)
{
	return g_quark_from_static_string (DAPI_GNOME_ERROR_DOMAIN);
}

gboolean
dapi_gnome_startup (void)
{
	static gboolean  inited = FALSE;
	EBook           *book;
	gboolean         ok;
 	gint             modules = 0;

	if (inited) {
		return TRUE;
	}

	dapi_debug (DEBUG_DOMAIN, "Starting up...");

	temporary_files = g_hash_table_new_full (g_str_hash,
						 g_str_equal,
						 (GDestroyNotify) g_free, 
						 NULL);

	dpms_clients = g_hash_table_new_full (g_direct_hash,
					      g_direct_equal,
					      NULL,
					      NULL);

	/* Check GnomeVFS */
	ok = gnome_vfs_init ();
	dapi_debug (DEBUG_DOMAIN, "---> GnomeVFS...%s", ok ? "OK" : "Failed");

	if (ok) {
 		modules |= GNOME_MODULE_GNOMEVFS; 
	}

	/* Check GTK+ */
	ok = gtk_init_check (0, NULL);
	dapi_debug (DEBUG_DOMAIN, "---> GTK+...%s", ok ? "OK" : "Failed");

	if (ok) {
		modules |= GNOME_MODULE_GTK;
	}

	/* Check Ebook */
	book = e_book_new_system_addressbook (NULL);
	dapi_debug (DEBUG_DOMAIN, "---> Ebook...%s", book ? "OK" : "Failed");
	
	if (book) {
		modules |= GNOME_MODULE_EBOOK;
		g_object_unref (book);
	}

	/* We set up a list of capabilities that we can use at run time */
#ifdef HAVE_GKSU
	capabilities |= DAPI_CAP_RUNASUSER;
#endif

#ifdef HAVE_GNOME_SCREENSAVER
	capabilities |= DAPI_CAP_SUSPENDSCREENSAVING;
#endif

	if (modules & GNOME_MODULE_GNOMEVFS) {
		capabilities |= DAPI_CAP_OPENURL;
		capabilities |= DAPI_CAP_EXECUTEURL;
		capabilities |= DAPI_CAP_MAILTO;
		capabilities |= DAPI_CAP_LOCALFILE;
		capabilities |= DAPI_CAP_UPLOADFILE;
		capabilities |= DAPI_CAP_REMOVETEMPORARYLOCALFILE;
	}

	if (modules & GNOME_MODULE_GTK) {
		capabilities |= DAPI_CAP_BUTTONORDER;
	}

	if (modules & GNOME_MODULE_EBOOK) {
		capabilities |= DAPI_CAP_ADDRESSBOOKLIST;
		capabilities |= DAPI_CAP_ADDRESSBOOKGETNAME;
		capabilities |= DAPI_CAP_ADDRESSBOOKGETEMAILS;
		capabilities |= DAPI_CAP_ADDRESSBOOKFINDBYNAME;
		capabilities |= DAPI_CAP_ADDRESSBOOKOWNER;
		capabilities |= DAPI_CAP_ADDRESSBOOKGETVCARD30;
	}

	inited = TRUE;
	dapi_debug (DEBUG_DOMAIN, "Ready!");

	return TRUE;
}

void
dapi_gnome_shutdown (void)
{
	dapi_debug (DEBUG_DOMAIN, "Shutting down...");

	gnome_vfs_shutdown ();
	dapi_debug (DEBUG_DOMAIN, "---> GnomeVFS...OK");

	g_hash_table_destroy (temporary_files);
	g_hash_table_destroy (dpms_clients);
}

gint
dapi_gnome_get_capabilities (void)
{
	return capabilities;
}

gboolean 
dapi_gnome_has_capability (DapiCapability capability)
{
 	return capabilities & capability;
}

static gchar * 
gnome_open_url_get_exec (const char *scheme) 
{ 
	GConfClient *gconf_client; 
	gchar       *path; 
	gchar       *exec; 
	
	gconf_client = gconf_client_get_default (); 
	if (!gconf_client) { 
		g_warning ("Could not get default gconf client"); 
		return NULL; 
	} 
	
	path = g_strdup_printf ("%s/%s/command", GCONF_URL_HANDLERS_PATH, scheme); 
	exec = gconf_client_get_string (gconf_client, path, NULL); 
	g_free (path); 
	
	return exec; 
} 

gboolean  
dapi_gnome_open_url (const gchar  *url,
		     GError      **error)
{ 
	gchar    *exec_format; 
	gchar    *exec; 
	gboolean  result = FALSE; 

	gnome_return_val_if_not_capable (DAPI_CAP_OPENURL, FALSE);

	g_return_val_if_fail (url != NULL && strlen (url) > 0, FALSE);
	
	dapi_debug (DEBUG_DOMAIN, "Opening URL:'%s'", url);

	if (!strchr (url, ':')) {
		gchar *str;

		str = g_strdup_printf (_("Scheme (e.g. http://) was not present in URL:'%s'"), url);
		dapi_debug (DEBUG_DOMAIN, str); 
		g_set_error (error, DAPI_GNOME_ERROR, result, str);
		g_free (str);

		return FALSE;
	}

	exec_format = gnome_open_url_get_exec ("http"); 

	if (strstr (exec_format, "%s")) {
		exec = g_strdup_printf (exec_format, url); 
	} else {
		exec = g_strdup_printf ("%s %s", exec_format, url);
	}

	dapi_debug (DEBUG_DOMAIN, "Using command:'%s'", exec); 
	result = g_spawn_command_line_async (exec, error);
	
	if (result) { 
		dapi_debug (DEBUG_DOMAIN, 
			    "Started browser successfully");
		result = TRUE;
	} else { 
		dapi_debug (DEBUG_DOMAIN, 
			    "Could not start browser process, error:'%s'", 
			    error && *error ? (*error)->message : ""); 
	} 
	
	g_free (exec); 
	g_free (exec_format); 
	
	return result; 
} 

gboolean 
dapi_gnome_execute_url (const gchar  *url,
			GError      **error)
{
	GnomeVFSResult result;

	gnome_return_val_if_not_capable (DAPI_CAP_EXECUTEURL, FALSE);

	g_return_val_if_fail (url != NULL && strlen (url) > 0, FALSE);
	
	result = gnome_vfs_url_show (url);

	if (result == GNOME_VFS_OK) { 
		dapi_debug (DEBUG_DOMAIN, 
			    "Executed URL:'%s' successfully", 
			    url);
	} else {
		const gchar *str;
		
		str = gnome_vfs_result_to_string (result);
		g_set_error (error, DAPI_GNOME_ERROR, result, str);

		dapi_debug (DEBUG_DOMAIN, 
			    "Could not executed URL:'%s', error:'%s'", 
			    url, 
			    error && *error ? (*error)->message : "");
	}

	return result == GNOME_VFS_OK;
}

gboolean 
dapi_gnome_button_order (gboolean  *alternative_button_order,
			 GError   **error)
{
	GtkSettings *settings;

	gnome_return_val_if_not_capable (DAPI_CAP_BUTTONORDER, FALSE);

	g_return_val_if_fail (alternative_button_order != NULL, FALSE);

	/* Default */
	*alternative_button_order = FALSE;

	/* This gets the default settings for the default screen */
	settings = gtk_settings_get_default ();
	if (!settings) {
		gchar *str;

		str = g_strdup_printf (_("Could not get GTK+ default settings"));

		g_set_error (error, DAPI_GNOME_ERROR, 1, str); 
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		return FALSE;
	}

	g_object_get (settings, 
		      "gtk-alternative-button-order", alternative_button_order, 
		      NULL);

	dapi_debug (DEBUG_DOMAIN, "Getting button order...%s", 
		    *alternative_button_order ? "Alternative (OK/Cancel)" : "Normal (Cancel/OK)");
	
	return TRUE;
}

gboolean 
dapi_gnome_run_as_user (const gchar  *user,
			const gchar  *command,
			GError      **error)
{
#ifdef HAVE_GKSU
	gchar    *exec;
	gboolean  result;

	gnome_return_val_if_not_capable (DAPI_CAP_RUNASUSER, FALSE);

	g_return_val_if_fail (user != NULL && strlen (user) > 0, FALSE);
	g_return_val_if_fail (command != NULL && strlen (command) > 0, FALSE);

	/* We can't check if the file exists or is executable here
	 * because if we do then it will be relative to this user not
	 * the user trying to run the command.
	 */
	exec = g_strdup_printf ("%s -u %s %s", HAVE_GKSU, user, command);
	result = g_spawn_command_line_async (exec, error);
	g_free (exec);

	dapi_debug (DEBUG_DOMAIN, 
		    "Running gksu for command:'%s' as user:'%s'...%s->'%s'", 
		    command, user, 
		    result ? "OK" : "Failed", 
		    error && *error ? (*error)->message : "No Errors"); 

	g_clear_error (error);

	/* gksu SHOULD return 0 or 1 based on success of failure but
	 * it seems to always return 0 regardless. So we are always
	 * successfull here as a result even if the filename we want
	 * to execute can not be found.
	 */

	return result;
#else
	gnome_return_val_if_not_capable (DAPI_CAP_RUNASUSER, FALSE);
#endif
}

static gboolean
gnome_suspend_screen_saving_timeout_cb (gpointer data)
{
	GError   *error = NULL;
	gboolean  result;
	gchar    *exec;

	exec = g_strdup_printf ("%s --poke", HAVE_GNOME_SCREENSAVER);
	result = g_spawn_command_line_async (exec, &error);

	dapi_debug (DEBUG_DOMAIN,
		    "Running '%s':... %s->'%s'",
		    exec,
		    result ? "OK" : "Failed",
		    error ? error->message : "No Errors");
	
	g_clear_error (&error);
	g_free (exec);

	return TRUE;
}

gboolean 
dapi_gnome_suspend_screen_saving (guint      client_id, 
				  gboolean   suspend,
				  GError   **error)
{
#ifdef HAVE_GNOME_SCREENSAVER
	static guint timeout_id = 0;

	if (suspend) {
		gpointer p;

		p = g_hash_table_lookup (dpms_clients, 
					 GUINT_TO_POINTER (client_id));
		if (p) {
			gchar *str;

			str = g_strdup_printf (_("Client %d has already requested to "
						 "suspend screen saving"),
					       client_id);

			g_set_error (error, DAPI_GNOME_ERROR, 1, str);
			dapi_debug (DEBUG_DOMAIN, str);
			g_free (str);

			return FALSE;
		}

		g_hash_table_insert (dpms_clients, 
				     GUINT_TO_POINTER (client_id), 
				     GUINT_TO_POINTER (1));

		if (timeout_id == 0) {
			/* Start timeouts */
			timeout_id = g_timeout_add (30000, 
						    gnome_suspend_screen_saving_timeout_cb, 
						    NULL);
		}
		
		dapi_debug (DEBUG_DOMAIN, "Started to ping the screensaver");
	} else {
		gboolean removed;

		removed = g_hash_table_remove (dpms_clients, 
					       GUINT_TO_POINTER (client_id));
		if (!removed) {
			gchar *str;

			str = g_strdup_printf (_("Client %d had not requested to "
						 "resume screen saving"),
					       client_id);
			g_set_error (error, DAPI_GNOME_ERROR, 2, str);
			dapi_debug (DEBUG_DOMAIN, str);
			g_free (str);

			return FALSE;
		}

		if (g_hash_table_size (dpms_clients) < 1) {
			/* Stop pinging the screensaver */
			if (timeout_id != 0) {
				g_source_remove (timeout_id);
				timeout_id = 0;

				dapi_debug (DEBUG_DOMAIN, "Stopped to ping the screensaver");
			}
		}
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Suspending screen saving: %s for client "
		    "with id:%d... (%d clients left)", 
		    suspend ? "enable" : "disable",
		    client_id,
		    g_hash_table_size (dpms_clients));
	
	return TRUE;
#else
	gnome_return_val_if_not_capable (DAPI_CAP_SUSPENDSCREENSAVING, FALSE);
#endif
}

gboolean 
dapi_gnome_mail_to (const gchar  *to,
		    const gchar  *cc,
		    const gchar  *bcc,
		    const gchar  *subject,
		    const gchar  *body,
		    const gchar **attachments,
		    GError      **error)
{
	GnomeVFSResult  result;
	GString        *url;
	gint            i;

	gnome_return_val_if_not_capable (DAPI_CAP_MAILTO, FALSE);

	url = g_string_new ("mailto:?");
	
 	dapi_debug (DEBUG_DOMAIN, "Sending mail with details:");

	if (to && strlen (to) > 0) {
		g_string_append_printf (url, "to=%s", to);
		dapi_debug (DEBUG_DOMAIN, "\t\tcc:'%s'", to);
	}

	if (cc && strlen (cc) > 0) {
		g_string_append_printf (url, "&cc=%s", cc);
		dapi_debug (DEBUG_DOMAIN, "\t\tcc:'%s'", cc);
	}

	if (bcc && strlen (bcc) > 0) {
		g_string_append_printf (url, "&bcc=%s", bcc);
		dapi_debug (DEBUG_DOMAIN, "\t\tbcc:'%s'", bcc);
	}

	if (subject && strlen (subject) > 0) {
		g_string_append_printf (url, "&subject=%s", subject);
		dapi_debug (DEBUG_DOMAIN, "\t\tsubject:'%s'", subject);
	}

	if (body && strlen (body) > 0) {
		g_string_append_printf (url, "&body=%s", body);
		dapi_debug (DEBUG_DOMAIN, "\t\tbody:'%s'", body);
	}

	if (attachments) {
		for (i = 0; attachments[i]; i++) {
			g_string_append_printf (url, "&attach=%s", attachments[i]);
			dapi_debug (DEBUG_DOMAIN, "\t\tattachment %2.2d:'%s'", i, attachments[i]);
		}
	}

	dapi_debug (DEBUG_DOMAIN, "Sending mail using url:'%s'", url->str);
	
	result = gnome_vfs_url_show (url->str);
	if (result == GNOME_VFS_OK) {
		dapi_debug (DEBUG_DOMAIN, 
			    "Executed URL:'%s' successfully", 
			    url->str);
	} else {
		const gchar *str;

		str = gnome_vfs_result_to_string (result);
		g_set_error (error, DAPI_GNOME_ERROR, result, str);

		dapi_debug (DEBUG_DOMAIN, 
			    "Could not execute URL:'%s', error:'%s'",
			    url->str,
			    error && *error ? (*error)->message : "");
	}

	g_string_free (url, TRUE);

	return result == GNOME_VFS_OK;
}

static const gchar *
gnome_vfs_xfer_phase_to_str (GnomeVFSXferPhase phase)
{
	switch (phase) {
	case GNOME_VFS_XFER_PHASE_INITIAL: 
		/* Initial phase */
		return "Initial";
	case GNOME_VFS_XFER_CHECKING_DESTINATION: 
		/* Checking if destination can handle move/copy */
		return "Checking Destination";
	case GNOME_VFS_XFER_PHASE_COLLECTING: 
		/* Collecting file list */
		return "Collecting";
	case GNOME_VFS_XFER_PHASE_READYTOGO: 
		/* File list collected (*) */
		return "Ready To Go";
	case GNOME_VFS_XFER_PHASE_OPENSOURCE: 
		/* Opening source file for reading */
		return "Open Source";
	case GNOME_VFS_XFER_PHASE_OPENTARGET: 
		/* Creating target file for copy */
		return "Open target";
	case GNOME_VFS_XFER_PHASE_COPYING: 
		/* Copying data from source to target (*) */
		return "Copying";
	case GNOME_VFS_XFER_PHASE_MOVING: 
		/* Moving file from source to target (*) */
		return "Moving";
	case GNOME_VFS_XFER_PHASE_READSOURCE: 
		/* Reading data from source file */
		return "Read Source";
	case GNOME_VFS_XFER_PHASE_WRITETARGET: 
		/* Writing data to target file */
		return "Write Target";
	case GNOME_VFS_XFER_PHASE_CLOSESOURCE: 
		/* Closing source file */
		return "Close Source";
	case GNOME_VFS_XFER_PHASE_CLOSETARGET: 
		/* Closing target file */
		return "Close Target";
	case GNOME_VFS_XFER_PHASE_DELETESOURCE: 
		/* Deleting source file */
		return "Delete Source";
	case GNOME_VFS_XFER_PHASE_SETATTRIBUTES: 
		/* Setting attributes on target file */
		return "Set Attributes";
	case GNOME_VFS_XFER_PHASE_FILECOMPLETED: 
		/* Go to the next file (*) */
		return "File Completed";
	case GNOME_VFS_XFER_PHASE_CLEANUP: 
		/* cleaning up after a move (removing source files, etc.) */
		return "Clean Up";
	case GNOME_VFS_XFER_PHASE_COMPLETED: 
		/* Operation finished (*) */
		return "Completed";
		
	default:
		break;
	}

	return "Unknown";
}

static gint
gnome_local_file_transfer_cb (GnomeVFSAsyncHandle      *handle,
			      GnomeVFSXferProgressInfo *info,
			      TransferFileData         *data)
{
	const gchar *str;

	switch (info->status) {
	case GNOME_VFS_XFER_PROGRESS_STATUS_OK:
		str = "OK";
		break;
	case GNOME_VFS_XFER_PROGRESS_STATUS_VFSERROR:
 		str = gnome_vfs_result_to_string (info->vfs_status);
		break;
	case GNOME_VFS_XFER_PROGRESS_STATUS_OVERWRITE:
		str = "Overwrite";
		break;
	default:
		str = "Unknown";
		break;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Transfer progress update, status:%d->'%s' phase:%d->'%s'...", 
		    info->status, str, info->phase, gnome_vfs_xfer_phase_to_str (info->phase));

	if (info->status == GNOME_VFS_XFER_PROGRESS_STATUS_VFSERROR &&
	    info->vfs_status != GNOME_VFS_OK) {
		if (!data->error) {
			g_set_error (&data->error, DAPI_GNOME_ERROR, 
				     info->vfs_status, str);
		}

		return 0;
	}

	if (info->phase == GNOME_VFS_XFER_PHASE_COMPLETED) {
		DapiGnomeLocalFileCallback  func;

		func = data->callback;

		dapi_debug (DEBUG_DOMAIN, 
			    "Transfer %s to destination:'%s'",
			    data->error ? "failed" : "success", 
			    data->filename);
		
		(func) (data->filename, data->error, data->user_data);

		if (data->flag) {
			/* Don't free data->filename here because we need to remember
			 * it in our hash table so that the remove local file
			 * function can check we have actually created it. We must
			 * ONLY remove local files we created according to the spec.
			 */
			g_hash_table_insert (temporary_files, 
					     data->filename, 
					     GINT_TO_POINTER (1));
		} else {
			g_free (data->filename);
		}
		
		g_clear_error (&data->error);
		g_free (data);
	}

	return 1;
}

void 
dapi_gnome_local_file (const gchar                *url,
		       const gchar                *local,
		       gboolean                    allow_download,
		       DapiGnomeLocalFileCallback  callback,
		       gpointer                    user_data)
{
	GnomeVFSURI         *source;
	GnomeVFSURI         *target = NULL;
	GnomeVFSAsyncHandle *handle;
	GnomeVFSResult       result;
	GList               *source_uri_list;
	GList               *target_uri_list;
	gboolean             is_temporary = FALSE;
	GError              *error = NULL;
	gchar               *str;
	TransferFileData    *data;

	gnome_return_if_not_capable (DAPI_CAP_LOCALFILE);
	g_return_if_fail (url != NULL && strlen (url) > 0);
	g_return_if_fail (local != NULL && strlen (local) > 0);
	g_return_if_fail (callback != NULL);

 	dapi_debug (DEBUG_DOMAIN, 
		    "Copying remote url:'%s' to local url:'%s' (allow download: %s)",
		    url, local, allow_download ? "yes" : "no");

	source = gnome_vfs_uri_new (url);
	
	/* If the URL is local already, we just return that */
	if (gnome_vfs_uri_is_local (source)) {
		str = g_strdup_printf (_("The remote URL:'%s' is already local"), url);
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
 		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		/* Not sure if we should add this to the hash table so
		 * we can remove it later with remove local file
		 */
		(callback)(url, error, user_data);

		g_clear_error (&error);
		gnome_vfs_uri_unref (source);

		return;
	}

	/* If the URL is remote and we can't download it, we just
	 * return 
	 */
	if (!allow_download) {
		str = g_strdup_printf (_("The remote URL:'%s' needs downloading "
					 "and you have not allowed downloading"),
				       url);
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
 		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(url, error, user_data);

		g_clear_error (&error);
		gnome_vfs_uri_unref (source);

		return;
	}

	/* If the target URI already exists, we generate another
	 * temporary file name and use that instead
	 */
	if (local) {
		target = gnome_vfs_uri_new (local);

		if (gnome_vfs_uri_exists (target)) {
			gnome_vfs_uri_unref (target);
			target = NULL;
		}
	}

	if (!target) {
		gchar *tmp_filename;
		gint   fd;
		
		fd = g_file_open_tmp ("dapi-gnome-XXXXXX", &tmp_filename, NULL);
		if (fd == -1) {
			str = g_strdup (_("Could not create temporary filename."));
			g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
			dapi_debug (DEBUG_DOMAIN, str);
			g_free (str);
			
			(callback)(url, error, user_data);
			
			g_clear_error (&error);
			gnome_vfs_uri_unref (source);
			
			return;
		}

 		dapi_debug (DEBUG_DOMAIN,
			    "Specified local URL already exists or wasn't given, creating "
			    "new temporary URL:'%s'...",
			    tmp_filename);
		
		target = gnome_vfs_uri_new (tmp_filename);
		g_free (tmp_filename);

		close (fd);

		is_temporary = TRUE;
	}

	data = g_new0 (TransferFileData, 1);

	data->filename = gnome_vfs_uri_to_string (target,
						  GNOME_VFS_URI_HIDE_NONE);

	data->flag = is_temporary;

	data->callback = callback;
	data->user_data = user_data;

	source_uri_list = g_list_append (NULL, source);
	target_uri_list = g_list_append (NULL, target);

	result = gnome_vfs_async_xfer (&handle,
				       source_uri_list,
				       target_uri_list,
				       GNOME_VFS_XFER_RECURSIVE,
				       GNOME_VFS_XFER_ERROR_MODE_QUERY,
 				       GNOME_VFS_XFER_OVERWRITE_MODE_QUERY,  
				       GNOME_VFS_PRIORITY_DEFAULT,
				       (GnomeVFSAsyncXferProgressCallback) 
				       gnome_local_file_transfer_cb,
				       data,
				       NULL, NULL); 

	g_list_foreach (source_uri_list, (GFunc) gnome_vfs_uri_unref, NULL);
	g_list_foreach (target_uri_list, (GFunc) gnome_vfs_uri_unref, NULL);
	
	g_list_free (source_uri_list);
	g_list_free (target_uri_list);

	if (result != GNOME_VFS_OK) {
		str = g_strdup (_("File transfer failed locally for an unknown reason."));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);
		
		(callback)(url, error, user_data);
		
		g_clear_error (&error);

		g_free (data->filename);
		g_free (data);

		return;
	}
}

static gint
gnome_upload_file_transfer_cb (GnomeVFSAsyncHandle      *handle,
			       GnomeVFSXferProgressInfo *info,
			       TransferFileData         *data)
{
	const gchar *str;

	switch (info->status) {
	case GNOME_VFS_XFER_PROGRESS_STATUS_OK:
		str = "OK";
		break;
	case GNOME_VFS_XFER_PROGRESS_STATUS_VFSERROR:
 		str = gnome_vfs_result_to_string (info->vfs_status);
		break;
	case GNOME_VFS_XFER_PROGRESS_STATUS_OVERWRITE:
		str = "Overwrite";
		break;
	default:
		str = "Unknown";
		break;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Transfer progress update, status:%d->'%s' phase:%d->'%s'...", 
		    info->status, str, info->phase, gnome_vfs_xfer_phase_to_str (info->phase));

	if (info->status == GNOME_VFS_XFER_PROGRESS_STATUS_VFSERROR &&
	    info->vfs_status != GNOME_VFS_OK) {
		if (!data->error) {
			g_set_error (&data->error, DAPI_GNOME_ERROR, 
				     info->vfs_status, str);
		}

		return 0;
	}

	if (info->phase == GNOME_VFS_XFER_PHASE_COMPLETED) {
		DapiGnomeUploadFileCallback func;
		
		func = data->callback;

		dapi_debug (DEBUG_DOMAIN,
			    "Transfer %s to destination:'%s'", 
			    data->error ? "failed" : "complete", 
			    data->filename);
		
		(func) (data->error, data->user_data);

		if (data->flag) {
			dapi_gnome_remove_temp_local_file (data->filename);
		}
		
		g_clear_error (&data->error);

		g_free (data->filename);
		g_free (data);
	}

	return 1;
}

void 
dapi_gnome_upload_file (const gchar                 *local,
			const gchar                 *url,
			gboolean                     remove_local,
			DapiGnomeUploadFileCallback  callback,
			gpointer                     user_data)
{
	GnomeVFSURI         *source;
	GnomeVFSURI         *target;
	GnomeVFSAsyncHandle *handle;
	GnomeVFSResult       result;
	GList               *source_uri_list;
	GList               *target_uri_list;
	GError              *error = NULL;
	gchar               *str;
	TransferFileData    *data;

	gnome_return_if_not_capable (DAPI_CAP_UPLOADFILE);
	g_return_if_fail (url != NULL && strlen (url) > 0);
	g_return_if_fail (local != NULL && strlen (local) > 0);
	g_return_if_fail (callback != NULL);

 	dapi_debug (DEBUG_DOMAIN, 
		    "Copying remote url:'%s' to local url:'%s' (remove local: %s)",
		    url, local, remove_local ? "yes" : "no");

	target = gnome_vfs_uri_new (url);
	
	/* If the URL is local already, we just return that */
	if (gnome_vfs_uri_is_local (target)) {
		str = g_strdup_printf (_("The remote URL:'%s' is already local"), url);
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
 		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		/* Not sure if we should add this to the hash table so
		 * we can remove it later with remove local file
		 */
		(callback)(error, user_data);

		g_clear_error (&error);
		gnome_vfs_uri_unref (target);

		return;
	}

	source = gnome_vfs_uri_new (local);

	if (!gnome_vfs_uri_exists (source)) {
		str = g_strdup_printf (_("The local URL:'%s' does not exist"), local);
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
 		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(error, user_data);

		g_clear_error (&error);
		
		gnome_vfs_uri_unref (target);
		gnome_vfs_uri_unref (source);
		
		return;
	}

	data = g_new0 (TransferFileData, 1);

	data->filename = gnome_vfs_uri_to_string (source, 
						  GNOME_VFS_URI_HIDE_NONE);

	data->flag = remove_local;

	data->callback = callback;
	data->user_data = user_data;

	source_uri_list = g_list_append (NULL, source);
	target_uri_list = g_list_append (NULL, target);

	result = gnome_vfs_async_xfer (&handle,
				       source_uri_list,
				       target_uri_list,
				       GNOME_VFS_XFER_RECURSIVE,
				       GNOME_VFS_XFER_ERROR_MODE_QUERY,
 				       GNOME_VFS_XFER_OVERWRITE_MODE_QUERY,  
				       GNOME_VFS_PRIORITY_DEFAULT,
				       (GnomeVFSAsyncXferProgressCallback) 
				       gnome_upload_file_transfer_cb,
				       data,
				       NULL, NULL); 

	g_list_foreach (source_uri_list, (GFunc) gnome_vfs_uri_unref, NULL);
	g_list_foreach (target_uri_list, (GFunc) gnome_vfs_uri_unref, NULL);
	
	g_list_free (source_uri_list);
	g_list_free (target_uri_list);

	if (result != GNOME_VFS_OK) {
		str = g_strdup (_("File transfer failed locally for an unknown reason."));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);
		
		(callback)(error, user_data);
		
		g_clear_error (&error);

		g_free (data->filename);
		g_free (data);

		return;
	}
}

gboolean
dapi_gnome_remove_temp_local_file (const gchar *filename)
{
	gnome_return_val_if_not_capable (DAPI_CAP_REMOVETEMPORARYLOCALFILE, FALSE);

	g_return_val_if_fail (filename != NULL && strlen (filename) > 0, FALSE);

	dapi_debug (DEBUG_DOMAIN, 
		    "Removing temporary local file:'%s'", 
		    filename);

	if (!g_hash_table_lookup (temporary_files, filename)) {
		dapi_debug (DEBUG_DOMAIN, 
			    "Filename:'%s' was not created by this daemon or has "
			    "already been removed, doing nothing.",
			    filename);
		return FALSE;
	}

	g_hash_table_remove (temporary_files, filename);
	
	return gnome_vfs_unlink (filename) == GNOME_VFS_OK;
}

/* FIXME: For some reason the e_book_async_get_contact() fails to work
 * properly so we get all contacts and go through them instead. This
 * is detailed in bug #349069.
 */ 
#undef USE_DIRECT_EBOOK_API
#ifdef USE_DIRECT_EBOOK_API

static void
gnome_ebook_get_names_cb (EBook       *book, 
			  EBookStatus  status, 
			  EContact    *contact, 
			  gpointer     closure)
{
	const gchar            *id;
	DapiGnomeNamesCallback  callback;
	gpointer                user_data;

	const gchar            *given_name = NULL;
	const gchar            *family_name = NULL;
	const gchar            *full_name = NULL;

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not get names for contact:'%s', error %d"),
				       id, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (id, NULL, NULL, NULL, error, user_data);
	
		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	dapi_debug (DEBUG_DOMAIN, 
		    "Getting address book names (given, family, full) "
		    "for user ID:'%s'...", 
		    id);
	
	given_name = e_contact_get_const (contact, E_CONTACT_GIVEN_NAME);
	family_name = e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);
	full_name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);

	(callback) (id, given_name, family_name, full_name, user_data);

 	g_object_unref (book); 
}

static void
gnome_ebook_get_email_addresses_cb (EBook       *book, 
				    EBookStatus  status, 
				    EContact    *contact, 
				    gpointer     closure)
{
	const gchar             *id;
	DapiGnomeEmailsCallback  callback;
	gpointer                 user_data;

	GError                  *error = NULL;
	GList                   *addresses = NULL;

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not get all email addresses "
					 "for contact:'%s', error %d"),
				       id, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (id, NULL, error, user_data);

		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Getting address book email addresses for user ID:'%s'...",
		    id);
	
	addresses = e_contact_get (contact, E_CONTACT_EMAIL);

	(callback) (id, addresses, NULL, user_data);

	g_list_foreach (addresses, (GFunc) g_free, NULL);
	g_list_free (addresses);

 	g_object_unref (book); 
}

#else  /* USE_DIRECT_EBOOK_API */

static void
gnome_ebook_get_list_cb (EBook       *book, 
			 EBookStatus  status, 
			 GList       *contacts, 
			 gpointer     closure)
{
	DapiGnomeListCallback  callback;
	gpointer               user_data;
	
	GError                *error = NULL;
	GList                 *ids = NULL;
	GList                 *l;

	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not get all contacts, error %d"),
				       status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (NULL, error, user_data);

		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	dapi_debug (DEBUG_DOMAIN, "Getting address book user IDs...");
	
	for (l = contacts; l; l = l->next) {
		gconstpointer p;

		p = e_contact_get_const (E_CONTACT (l->data), E_CONTACT_UID);
		ids = g_list_prepend (ids, (gpointer) p);
	}

	dapi_debug (DEBUG_DOMAIN, "Found %d user IDs", g_list_length (ids));

	(callback)(ids, NULL, user_data);

	g_list_free (ids);
 	g_object_unref (book); 
}

static void
gnome_ebook_get_names_cb (EBook       *book, 
			  EBookStatus  status, 
			  GList       *contacts, 
			  gpointer     closure)
{
	const gchar           *id;
	DapiGnomeNameCallback  callback;
	gpointer               user_data;
	GList                 *l;

	GError                *error = NULL;
	const gchar           *given_name = NULL;
	const gchar           *family_name = NULL;
	const gchar           *full_name = NULL;
	
	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not get names for contact:'%s', error %d"),
				       id, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (id, NULL, NULL, NULL, error, user_data);
	
		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Getting address book names for user ID:'%s'...",
		    id);
	
	for (l = contacts; l; l = l->next) {
		EContact    *contact;
		const gchar *this_id;

		contact = E_CONTACT (l->data);
		this_id = e_contact_get_const (contact, E_CONTACT_UID);

		if (this_id && strcmp (this_id, id) == 0) {
			given_name = e_contact_get_const (contact, E_CONTACT_GIVEN_NAME);
			family_name = e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);
			full_name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);
			break;
		}
	}

	if (given_name || family_name || full_name) {
		dapi_debug (DEBUG_DOMAIN, 
			    "Found given name:'%s', family name:'%s', "
			    "full name:'%s' for user ID:'%s'", 
			    given_name ? given_name : "",
			    family_name ? family_name : "",
			    full_name ? full_name : "", 
			    id);
	} else {
		dapi_debug (DEBUG_DOMAIN, 
			    "Found no names related to the user ID:'%s'", 
			    id);
	}

	(callback) (id, given_name, family_name, full_name, NULL, user_data);

 	g_object_unref (book); 
}

static void
gnome_ebook_get_email_addresses_cb (EBook       *book, 
				    EBookStatus  status, 
				    GList       *contacts, 
				    gpointer     closure)
{
	const gchar             *id;
	DapiGnomeEmailsCallback  callback;
	gpointer                 user_data;
	
	GError                  *error = NULL;
	GList                   *addresses = NULL;
	GList                   *l;

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not email addresses contact:'%s', error %d"),
				       id, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (id, NULL, error, user_data);
	
		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Getting address book email addresses for user ID:'%s'...",
		    id);
	
	for (l = contacts; l; l = l->next) {
		EContact    *contact;
		const gchar *this_id;

		contact = E_CONTACT (l->data);
		this_id = e_contact_get_const (contact, E_CONTACT_UID);

		if (this_id && strcmp (this_id, id) == 0) {
			addresses = e_contact_get (contact, E_CONTACT_EMAIL);
			break;
		}
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Found %d email addresses for user ID:'%s'", 
		    g_list_length (addresses), id);

	(callback) (id, addresses, NULL, user_data);

	g_list_foreach (addresses, (GFunc) g_free, NULL);
	g_list_free (addresses);

 	g_object_unref (book); 
}

static void
gnome_ebook_get_vcard_cb (EBook       *book, 
			  EBookStatus  status, 
			  GList       *contacts, 
			  gpointer     closure)
{
	const gchar            *id;
	DapiGnomeVCardCallback  callback;
	gpointer                user_data;

	GError                 *error = NULL;
	GList                  *l;
	gchar                  *vcard = NULL;

	id = g_object_get_data (G_OBJECT (book), "id");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not get vcard for contact:'%s', error %d"),
				       id, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (id, NULL, error, user_data);
	
		g_clear_error (&error);
		g_object_unref (book); 

		return;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Getting address book vcard for user ID:'%s'...",
		    id);
	
	for (l = contacts; l; l = l->next) {
		EContact    *contact;
		const gchar *this_id;

		contact = E_CONTACT (l->data);
		this_id = e_contact_get_const (contact, E_CONTACT_UID);

		if (this_id && strcmp (this_id, id) == 0) {
			vcard = e_vcard_to_string (E_VCARD (contact), 
						   EVC_FORMAT_VCARD_30);
			break;
		}
	}

	/* Should we use the GError if the vcard is empty? */
	dapi_debug (DEBUG_DOMAIN, 
		    "%s vcard for user ID:'%s'", 
		    vcard ? "Found" : "Could not find", id);

	(callback) (id, vcard, NULL, user_data);

	g_free (vcard);

 	g_object_unref (book); 
}

#endif /* USE_DIRECT_EBOOK_API */

static void
gnome_ebook_find_by_name_cb (EBook       *book, 
			     EBookStatus  status, 
			     GList       *contacts, 
			     gpointer     closure)
{
	const gchar           *name;
	DapiGnomeFindCallback  callback;
	gpointer               user_data;

	GError                *error = NULL;
	GList                 *ids = NULL;
	GList                 *l;

	name = g_object_get_data (G_OBJECT (book), "name");
	callback = g_object_get_data (G_OBJECT (book), "callback");
	user_data = g_object_get_data (G_OBJECT (book), "userdata");

	if (status != E_BOOK_ERROR_OK) {
		gchar *str;

		str = g_strdup_printf (_("Could not find contact by name:'%s', error %d"),
				       name, status);
		g_set_error (&error, DAPI_GNOME_ERROR, status, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback) (name, NULL, error, user_data);

		g_clear_error (&error);
		g_object_unref (book); 
		
		return;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "Finding address book user IDs by name:'%s'...", 
		    name);
	
	for (l = contacts; l; l = l->next) {
		EContact    *contact;
		const gchar *this_name;

		contact = E_CONTACT (l->data);
		this_name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);

		if (this_name && strcmp (this_name, name) == 0) {
			dapi_debug (DEBUG_DOMAIN, "\t\t'%s'...YES", this_name);
			ids = g_list_prepend 
				(ids, (gpointer) e_contact_get_const (contact, E_CONTACT_UID));
			break;
		} else {
			dapi_debug (DEBUG_DOMAIN, "\t\t'%s'...NO", this_name);
		}
	}

	(callback) (name, ids, NULL, user_data);

	g_list_free (ids);

 	g_object_unref (book); 
}

static void
gnome_ebook_loaded_cb (EBook       *book, 
		       EBookStatus  status, 
		       gpointer     closure)
{
	EBookQuery        *query;
	EBookListCallback  func;

	func = g_object_get_data (G_OBJECT (book), "func");

	if (status != E_BOOK_ERROR_OK) {
		g_warning ("Could not load book, status is %d", status);
		return;
	}

	query = e_book_query_field_exists (E_CONTACT_FULL_NAME);
	e_book_async_get_contacts (book, query, 
				   (EBookListCallback) func, 
				   NULL);
	e_book_query_unref (query);
}

void
dapi_gnome_address_book_get_list (DapiGnomeListCallback callback,
				  gpointer              user_data)
{
	EBook  *book;
	GError *error = NULL;

	gnome_return_if_not_capable (DAPI_CAP_ADDRESSBOOKLIST);
	g_return_if_fail (callback != NULL);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		gchar *str;

		str = g_strdup (_("Could not get system address book"));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(NULL, error, user_data);

		g_clear_error (&error);

		return;
	}

	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
	g_object_set_data (G_OBJECT (book), "func", gnome_ebook_get_list_cb);
	
 	e_book_async_open (book, FALSE,
			   (EBookCallback) gnome_ebook_loaded_cb, NULL); 
}

void
dapi_gnome_address_book_get_name (const gchar            *id,
				  DapiGnomeNameCallback  callback,
				  gpointer               user_data)
{
	EBook  *book;
	GError *error = NULL;

	gnome_return_if_not_capable (DAPI_CAP_ADDRESSBOOKGETNAME);
	g_return_if_fail (id != NULL && strlen (id) > 0);
	g_return_if_fail (callback != NULL);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		gchar *str;

		str = g_strdup (_("Could not get system address book"));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(id, NULL, NULL, NULL, error, user_data);

		g_clear_error (&error);

		return;
	}

	g_object_set_data_full (G_OBJECT (book), "id", g_strdup (id), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
 	g_object_set_data (G_OBJECT (book), "func", gnome_ebook_get_names_cb); 
	
 	e_book_async_open (book, FALSE,
			   (EBookCallback) gnome_ebook_loaded_cb, NULL); 
}

void
dapi_gnome_address_book_get_emails (const gchar             *id,
				    DapiGnomeEmailsCallback  callback,
				    gpointer                 user_data)
{
	EBook  *book;
	GError *error = NULL;

	gnome_return_if_not_capable (DAPI_CAP_ADDRESSBOOKGETEMAILS);
	g_return_if_fail (id != NULL && strlen (id) > 0);
	g_return_if_fail (callback != NULL);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		gchar *str;

		str = g_strdup (_("Could not get system address book"));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(id, NULL, error, user_data);

		g_clear_error (&error);

		return;
	}

	g_object_set_data_full (G_OBJECT (book), "id", g_strdup (id), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
	g_object_set_data (G_OBJECT (book), "func", gnome_ebook_get_email_addresses_cb);
	
 	e_book_async_open (book, FALSE, 
			   (EBookCallback) gnome_ebook_loaded_cb, NULL); 
}

void
dapi_gnome_address_book_find_by_name (const gchar           *name,
				      DapiGnomeFindCallback  callback,
				      gpointer               user_data)
{
	EBook  *book;
	GError *error = NULL;

	gnome_return_if_not_capable (DAPI_CAP_ADDRESSBOOKFINDBYNAME);
	g_return_if_fail (name != NULL && strlen (name) > 0);
	g_return_if_fail (callback != NULL);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		gchar *str;

		str = g_strdup (_("Could not get system address book"));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(name, NULL, error, user_data);

		g_clear_error (&error);

		return;
	}

	g_object_set_data_full (G_OBJECT (book), "name", g_strdup (name), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
	g_object_set_data (G_OBJECT (book), 
			   "func", gnome_ebook_find_by_name_cb);
	
 	e_book_async_open (book, FALSE, 
			   (EBookCallback) gnome_ebook_loaded_cb, NULL); 
}

gboolean
dapi_gnome_address_book_get_owner (gchar  **id,
				   GError **error)
{
	EBook    *book;
	EContact *contact;

	gnome_return_val_if_not_capable (DAPI_CAP_ADDRESSBOOKOWNER, FALSE);

	g_return_val_if_fail (id != NULL, FALSE);

	if (!e_book_get_self (&contact, &book, error)) {
		dapi_debug (DEBUG_DOMAIN, "Could not get own contact id, %s",
			    error && *error ? (*error)->message : "No Errors");
		return FALSE;
	}

	*id = e_contact_get (contact, E_CONTACT_UID);

	g_object_unref (contact);
	g_object_unref (book);

	return TRUE;
}

void
dapi_gnome_address_book_get_vcard (const gchar            *id,
				   DapiGnomeVCardCallback  callback,
				   gpointer                user_data)
{
	EBook  *book;
	GError *error = NULL;
 
	gnome_return_if_not_capable (DAPI_CAP_ADDRESSBOOKGETVCARD30);
	g_return_if_fail (id != NULL && strlen (id) > 0);
	g_return_if_fail (callback != NULL);

	book = e_book_new_system_addressbook (NULL);
	if (!book) {
		gchar *str;

		str = g_strdup (_("Could not get system address book"));
		g_set_error (&error, DAPI_GNOME_ERROR, 1, str);
		dapi_debug (DEBUG_DOMAIN, str);
		g_free (str);

		(callback)(id, NULL, error, user_data);

		g_clear_error (&error);

		return;
	}

	g_object_set_data_full (G_OBJECT (book), "id", g_strdup (id), g_free);
	g_object_set_data (G_OBJECT (book), "callback", callback);
	g_object_set_data (G_OBJECT (book), "userdata", user_data);
	g_object_set_data (G_OBJECT (book), 
			   "func", gnome_ebook_get_vcard_cb);
	
 	e_book_async_open (book, FALSE,
			   (EBookCallback) gnome_ebook_loaded_cb, NULL); 
}

