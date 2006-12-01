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

#include <stdlib.h>
#include <string.h>

#include <glib/gi18n.h>

#include "dapi-debug.h"
#include "dapi-command.h"
#include "dapi-gnome.h"

#define DEBUG_DOMAIN "Command"

#define DAPI_COMMAND_ERROR_DOMAIN "DAPI"

#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DAPI_TYPE_COMMAND, DapiCommandPriv))

#define NEW_DATA(data, d1, d2, d3)		\
	data = g_new0 (CommandData, 1);		\
						\
	data->data1 = d1;			\
	data->data2 = d2; 			\
	data->data3 = d3;

#define command_async_return_if_fail(expr,context)			\
	G_STMT_START {							\
		if G_LIKELY(expr) { } else {				\
			GError *error = NULL;				\
									\
			g_set_error (&error,				\
				     DAPI_GNOME_ERROR,			\
				     0,					\
				     _("Assertion `%s' failed"),	\
				     #expr);				\
									\
			dbus_g_method_return_error (context, error);	\
			g_clear_error (&error);				\
									\
			return;						\
		};							\
	} G_STMT_END

#define command_return_val_if_fail(expr,val,error)			\
	G_STMT_START {							\
		if G_LIKELY(expr) { } else {				\
			g_set_error (error,				\
				     DAPI_GNOME_ERROR,			\
				     0,					\
				     _("Assertion `%s' failed"),	\
				     #expr);				\
									\
			return val;					\
		};							\
	} G_STMT_END

typedef struct {
	DBusGProxy *fd_proxy;
	guint       request_id;
} DapiCommandPriv;

typedef struct {
	gpointer data1;
	gpointer data2;
	gpointer data3;
} CommandData;

static void  dbus_finalize                        (GObject     *obj);
static guint command_get_next_request_id          (DapiCommand *obj);
static void  command_local_file_cb                (const gchar *filename,
						   GError      *error,
						   gpointer     user_data);
static void  command_upload_file_cb               (GError      *error,
						   gpointer     user_data);
static void  command_address_book_list_cb         (GList       *ids,
						   GError      *error,
						   gpointer     user_data);
static void  command_address_book_find_by_name_cb (const gchar *name,
						   GList       *ids,
						   GError      *error,
						   gpointer     user_data);
static void  command_address_book_get_name_cb     (const gchar *id,
						   const gchar *given_name,
						   const gchar *family_name,
						   const gchar *full_name,
						   GError      *error,
						   gpointer     user_data);
static void  command_address_book_get_emails_cb   (const gchar *id,
						   GList       *emails,
						   GError      *error,
						   gpointer     user_data);
static void  command_address_book_get_vcard_cb    (const gchar *id,
						   const gchar *vcard,
						   GError      *error,
						   gpointer     user_data);

G_DEFINE_TYPE(DapiCommand, dapi_command, G_TYPE_OBJECT)

static void
dapi_command_class_init (DapiCommandClass *class)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (class);

	object_class->finalize = dbus_finalize;

	g_type_class_add_private (object_class, sizeof (DapiCommandPriv));
}

static void
dapi_command_init (DapiCommand *obj)
{
}

static void
dbus_finalize (GObject *obj)
{
	DapiCommandPriv *priv;

	priv = GET_PRIV (obj);

	dapi_debug (DEBUG_DOMAIN, "Shutting down...");

	if (priv->fd_proxy) {
		g_object_unref (priv->fd_proxy);
	}

	G_OBJECT_CLASS (dapi_command_parent_class)->finalize (obj);
}

GQuark
dapi_command_error_quark (void)
{
	return g_quark_from_static_string (DAPI_COMMAND_ERROR_DOMAIN);
}

static guint
command_get_next_request_id (DapiCommand *obj)
{
	DapiCommandPriv *priv;

	priv = GET_PRIV (obj);

	return ++(priv->request_id);
}

/*
 * Functions
 */
gboolean
dapi_command_capabilities (DapiCommand  *obj,
			   gint         *capabilities,
			   GError      **error)
{
	guint request_id;

	command_return_val_if_fail (capabilities != NULL, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for capability set", 
		    request_id);

 	*capabilities = dapi_gnome_get_capabilities ();

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] Success", 
		    request_id);
	
	return TRUE;
}

gboolean
dapi_command_open_url (DapiCommand  *obj,
		       const gchar  *url,
		       GValue       *windowinfo,
		       GError      **error)
{
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (url != NULL && strlen (url) > 0, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to open URL:'%s'",
		    request_id, url);

	result = dapi_gnome_open_url (url, error);
	
	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error:'%s'",
		    request_id, 
		    result ? "Success" : "Failed",
		    error && *error ? (*error)->message : "");

	return result;
}

gboolean
dapi_command_execute_url (DapiCommand  *obj,
			  const gchar  *url,
			  GValue       *windowinfo,
			  GError      **error)
{
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (url != NULL && strlen (url) > 0, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to execute URL:'%s'",
		    request_id, url);

	result = dapi_gnome_execute_url (url, error);
	
	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error:'%s'",
		    request_id, 
		    result ? "Success" : "Failed",
		    error && *error ? (*error)->message : "");

	return result;
}

gboolean
dapi_command_button_order (DapiCommand  *obj,
			   guint        *order,
			   GError      **error)
{
	gboolean alternative_order;
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (order != NULL, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for button order",
		    request_id);

	result = dapi_gnome_button_order (&alternative_order, error);
	
	/* We use a boolean because there are only 2 orders the
	 * buttons can be in, and the spec says we return:
	 *    0 for error,
	 *    1 for cancel/ok.
	 *    2 for ok/cancel,
	 * 
	 * We don't return 0, since the error is propagated by DBus.
	 */
	if (alternative_order) {
		*order = 2;
	} else {
		*order = 1;
	}

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s (button order:'%s'), error:'%s'",
		    request_id,
		    result ? "Success" : "Failed",
		    alternative_order ? "alternative" : "normal",
		    error && *error ? (*error)->message : "");

	return result;
}

gboolean
dapi_command_run_as_user (DapiCommand  *obj,
			  const gchar  *user,
			  const gchar  *command,
			  GValue       *windowinfo,
			  GError      **error)
{
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (user != NULL && strlen (user) > 0, FALSE, error);
	command_return_val_if_fail (command != NULL && strlen (user) > 0, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to run command:'%s' as user:'%s'", 
		    request_id, command, user);

	result = dapi_gnome_run_as_user (user, command, error);
		
	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error:'%s'",
		    request_id,
		    result ? "Success" : "Failed",
		    error && *error ? (*error)->message : "");
	
	return result;
}

gboolean
dapi_command_suspend_screen_saving (DapiCommand  *obj,
				    guint         client_id,
				    gboolean      suspend,
				    GError      **error)
{
	gboolean result;
	guint    request_id;

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to %s screen saving from client id:%d", 
		    request_id, 
		    suspend ? "suspend" : "cancel suspend", 
		    client_id);

	result = dapi_gnome_suspend_screen_saving (client_id, suspend, error);

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s",
		    request_id,
		    result ? "Success" : "Failed");
	
	return result;
}

gboolean
dapi_command_mail_to (DapiCommand  *obj,
		      const gchar  *subject,
		      const gchar  *body,
		      const gchar  *to,
		      const gchar  *cc,
		      const gchar  *bcc,
		      const gchar **attachments,
		      GValue       *windowinfo,
		      GError      **error)
{
	gboolean result;
	guint    request_id;

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to send mail to:'%s' with subject:'%s', cc:'%s', bcc:'%s'", 
		    request_id, to, subject, cc, bcc);
	
	result = dapi_gnome_mail_to (to, cc, bcc, subject, body, attachments, error);

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s", 
		    request_id, result ? "Success" : "Failed");

	return result;
}

static void
command_local_file_cb (const gchar *filename,
		       GError      *error,
		       gpointer     user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		dbus_g_method_return (context, filename);
	}

	g_free (data);
}

void
dapi_command_local_file (DapiCommand            *obj,
			 const gchar            *url,
			 const gchar            *local,
			 gboolean                allow_download,
			 GValue                 *windowinfo,
			 DBusGMethodInvocation  *context,
			 GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (url != NULL && strlen (url) > 0, context);
	command_async_return_if_fail (local != NULL && strlen (local) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to get file:'%s' and save as:'%s', %sallowing download", 
		    request_id, url, local, allow_download ? "" : "not ");

	dapi_gnome_local_file (url, local, allow_download, command_local_file_cb, data);
}

static void
command_upload_file_cb (GError      *error,
			gpointer     user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		dbus_g_method_return (context);
	}

	g_free (data);
}

void
dapi_command_upload_file (DapiCommand            *obj,
			  const gchar            *local,
			  const gchar            *url,
			  gboolean                remove_local,
			  GValue                 *windowinfo,
			  DBusGMethodInvocation  *context,
			  GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (local != NULL && strlen (local) > 0, context);
	command_async_return_if_fail (url != NULL && strlen (url) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to upload file:'%s' to:'%s', %sremoving local file", 
		    url, local, remove_local ? "" : "not ");

	dapi_gnome_upload_file (url, local, remove_local, command_upload_file_cb, data);
}

gboolean
dapi_command_remove_temporary_local_file (DapiCommand  *obj,
					  const gchar  *local,
					  GError      **error)
{
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (local != NULL && strlen (local) > 0, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request to remove temporary local file:'%s'", 
		    request_id, local);

	result = dapi_gnome_remove_temp_local_file (local);

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error && *error ? (*error)->message : "no error given");

	return result;
}

static void
command_address_book_list_cb (GList    *ids,
			      GError   *error,
			      gpointer  user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;
	GList                 *l;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		gchar **strs;
		gint    i;

		strs = g_new (gchar*, g_list_length (ids) + 1);
		for (l = ids, i = 0; l; l = l->next, i++) {
			strs[i] = g_strdup (l->data);
		}

		strs[i] = NULL;
		dbus_g_method_return (context, strs);
		g_strfreev (strs);
	}

	g_free (data);
}

void
dapi_command_address_book_list (DapiCommand            *obj,
				DBusGMethodInvocation  *context,
				GError                **error)
{
	CommandData *data;
	guint        request_id;

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for address book list",
		    request_id);

	dapi_gnome_address_book_get_list (command_address_book_list_cb, data);
}

gboolean
dapi_command_address_book_owner (DapiCommand  *obj,
				 gchar       **contact_id,
				 GError      **error)
{
	gboolean result;
	guint    request_id;

	command_return_val_if_fail (contact_id != NULL, FALSE, error);

	request_id = command_get_next_request_id (obj);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> Request for address book owner");

	result = dapi_gnome_address_book_get_owner (contact_id, error);

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error:'%s'",
		    request_id,
		    result ? "Success" : "Failed",
		    error && *error ? (*error)->message : "no error given");

	return result;
}

static void
command_address_book_find_by_name_cb (const gchar *name,
				      GList       *ids,
				      GError      *error,
				      gpointer     user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		GList  *l;
		gchar **strs;
		gint    i;

		strs = g_new (gchar*, g_list_length (ids) + 1);
		for (l = ids, i = 0; l; l = l->next, i++) {
			strs[i] = g_strdup (l->data);
		}

		strs[i] = NULL;
		dbus_g_method_return (context, strs);
		g_strfreev (strs);
	}

	g_free (data);
}

void
dapi_command_address_book_find_by_name (DapiCommand            *obj,
					const gchar            *name,
					DBusGMethodInvocation  *context,
					GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (name != NULL && strlen (name) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for address book contact by name:'%s'",
		    request_id, name);

	dapi_gnome_address_book_find_by_name (name, command_address_book_find_by_name_cb, data);
}

static void
command_address_book_get_name_cb (const gchar *id,
				  const gchar *given_name,
				  const gchar *family_name,
				  const gchar *full_name,
				  GError   *error,
				  gpointer  user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		dbus_g_method_return (context, given_name, family_name, full_name);
	}

	g_free (data);
}

void
dapi_command_address_book_get_name (DapiCommand            *obj,
				    const gchar            *contact_id,
				    DBusGMethodInvocation  *context,
				    GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (contact_id != NULL && strlen (contact_id) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for address book name for contact:'%s'",
		    request_id, contact_id);

	dapi_gnome_address_book_get_name (contact_id, command_address_book_get_name_cb, data);
}

static void
command_address_book_get_emails_cb (const gchar  *id,
				    GList        *emails,
				    GError       *error,
				    gpointer      user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		GList  *l;
		gchar **strs;
		gint    i;

		strs = g_new (gchar*, g_list_length (emails) + 1);
		for (l = emails, i = 0; l; l = l->next, i++) {
			strs[i] = g_strdup (l->data);
		}

		strs[i] = NULL;
		dbus_g_method_return (context, strs);
		g_strfreev (strs);
	}

	g_free (data);
}

void
dapi_command_address_book_get_emails (DapiCommand            *obj,
				      const gchar            *contact_id,
				      DBusGMethodInvocation  *context,
				      GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (contact_id != NULL && strlen (contact_id) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for address book email addresses for contact:'%s'",
		    request_id, contact_id);

	dapi_gnome_address_book_get_emails (contact_id, command_address_book_get_emails_cb, data);
}

static void
command_address_book_get_vcard_cb (const gchar *id,
				   const gchar *vcard,
				   GError      *error,
				   gpointer     user_data)
{
	DBusGMethodInvocation *context;
	CommandData           *data;
	guint                  request_id;

	data = user_data;

	request_id = GPOINTER_TO_UINT (data->data1);
	context = data->data2;

	dapi_debug (DEBUG_DOMAIN, 
		    "<--- [%d] %s, error: %s", 
		    request_id,
		    error ? "Failed" : "Success",
		    error ? error->message : "no error given");

	if (error) {
		dbus_g_method_return_error (context, error);
	} else {
		dbus_g_method_return (context, vcard);
	}

	g_free (data);
}

void
dapi_command_address_book_get_vcard30 (DapiCommand            *obj,
				       const gchar            *contact_id,
				       DBusGMethodInvocation  *context,
				       GError                **error)
{
	CommandData *data;
	guint        request_id;

	command_async_return_if_fail (contact_id != NULL && strlen (contact_id) > 0, context);

	request_id = command_get_next_request_id (obj);

	NEW_DATA(data, GUINT_TO_POINTER (request_id), context, NULL);

	dapi_debug (DEBUG_DOMAIN, 
		    "---> [%d] Request for address book vcard for contact:'%s'",
		    request_id, contact_id);

	dapi_gnome_address_book_get_vcard (contact_id, command_address_book_get_vcard_cb, data);
}

