/* -*- mode: C; c-file-style: "gnu" -*- */
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

#ifndef __DAPI_COMMAND_H__
#define __DAPI_COMMAND_H__

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <dbus/dbus-glib-lowlevel.h>

#define DAPI_SERVICE                "org.freedesktop.dapi"
#define DAPI_PATH                   "/org/freedesktop/dapi"
#define DAPI_INTERFACE              "org.freedesktop.dapi"

G_BEGIN_DECLS

#define DAPI_TYPE_COMMAND           (dapi_command_get_type ())
#define DAPI_COMMAND(object)        (G_TYPE_CHECK_INSTANCE_CAST ((object), DAPI_TYPE_COMMAND, DapiCommand))
#define DAPI_COMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass), DAPI_TYPE_COMMAND, DapiCommandClass))
#define DAPI_IS_OBJECT(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), DAPI_TYPE_COMMAND))
#define DAPI_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DAPI_TYPE_COMMAND))
#define DAPI_COMMAND_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), DAPI_TYPE_COMMAND, DapiCommandClass))

#define DAPI_COMMAND_ERROR          dapi_command_error_quark()

typedef struct DapiCommand      DapiCommand;
typedef struct DapiCommandClass DapiCommandClass;

struct DapiCommand {
	GObject parent;
};

struct DapiCommandClass {
	GObjectClass parent;
};

GType    dapi_command_get_type                    (void);
GQuark   dapi_command_error_quark                 (void) G_GNUC_CONST;
gboolean dapi_command_capabilities                (DapiCommand            *obj,
						   gint                   *capabilities,
						   GError                **error);
gboolean dapi_command_open_url                    (DapiCommand            *obj,
						   const gchar            *url,
						   GValue                 *windowinfo,
						   GError                **error);
gboolean dapi_command_execute_url                 (DapiCommand            *obj,
						   const gchar            *url,
						   GValue                 *windowinfo,
						   GError                **error);
gboolean dapi_command_button_order                (DapiCommand            *obj,
						   guint                  *order,
						   GError                **error);
gboolean dapi_command_run_as_user                 (DapiCommand            *obj,
						   const gchar            *user,
						   const gchar            *command,
						   GValue                 *windowinfo,
						   GError                **error);
gboolean dapi_command_suspend_screen_saving       (DapiCommand            *obj,
						   guint                   client_id,
						   gboolean                suspend,
						   GError                **error);
gboolean dapi_command_mail_to                     (DapiCommand            *obj,
						   const gchar            *subject,
						   const gchar            *body,
						   const gchar            *to,
						   const gchar            *cc,
						   const gchar            *bcc,
						   const gchar           **attachments,
						   GValue                 *windowinfo,
						   GError                **error);
void     dapi_command_local_file                  (DapiCommand            *obj,
						   const gchar            *url,
						   const gchar            *local,
						   gboolean                allow_download,
						   GValue                 *windowinfo,
						   DBusGMethodInvocation  *context,
						   GError                **error);
void     dapi_command_upload_file                 (DapiCommand            *obj,
						   const gchar            *local,
						   const gchar            *url,
						   gboolean                remove_local,
						   GValue                 *windowinfo,
						   DBusGMethodInvocation  *context,
						   GError                **error);
gboolean dapi_command_remove_temporary_local_file (DapiCommand            *obj,
						   const gchar            *local,
						   GError                **error);
void     dapi_command_address_book_list           (DapiCommand            *obj,
						   DBusGMethodInvocation  *context,
						   GError                **error);
gboolean dapi_command_address_book_owner          (DapiCommand            *obj,
						   gchar                 **contact_id,
						   GError                **error);
void     dapi_command_address_book_find_by_name   (DapiCommand            *obj,
						   const gchar            *name,
						   DBusGMethodInvocation  *context,
						   GError                **error);
void     dapi_command_address_book_get_name       (DapiCommand            *obj,
						   const gchar            *contact_id,
						   DBusGMethodInvocation  *context,
						   GError                **error);
void     dapi_command_address_book_get_emails     (DapiCommand            *obj,
						   const gchar            *contact_id,
						   DBusGMethodInvocation  *context,
						   GError                **error);
void     dapi_command_address_book_get_vcard30    (DapiCommand            *obj,
						   const gchar            *contact_id,
						   DBusGMethodInvocation  *context,
						   GError                **error);
G_END_DECLS

#endif /* __DAPI_COMMAND_H__ */
