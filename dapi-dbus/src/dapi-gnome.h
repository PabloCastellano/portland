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

#ifndef __DAPI_GNOME_H__
#define __DAPI_GNOME_H__

#include <glib.h>

#include <dapi-common.h>

#define DAPI_GNOME_ERROR dapi_gnome_error_quark()

GQuark   dapi_gnome_error_quark               (void);
gboolean dapi_gnome_startup                   (void);
void     dapi_gnome_shutdown                  (void);
gboolean dapi_gnome_has_capability            (DapiCapability                capability);
gint     dapi_gnome_get_capabilities          (void);
gboolean dapi_gnome_open_url                  (const gchar                  *url,
					       GError                      **error);
gboolean dapi_gnome_execute_url               (const gchar                  *url,
					       GError                      **error);
gboolean dapi_gnome_button_order              (gboolean                     *alternative_button_order,
					       GError                      **error);
gboolean dapi_gnome_run_as_user               (const gchar                  *user,
					       const gchar                  *command,
					       GError                      **error);
gboolean dapi_gnome_suspend_screen_saving     (guint                         client_id,
					       gboolean                      suspend,
					       GError                      **error);
gboolean dapi_gnome_mail_to                   (const gchar                  *to,
					       const gchar                  *cc,
					       const gchar                  *bcc,
					       const gchar                  *subject,
					       const gchar                  *body,
					       const gchar                 **attachments,
					       GError                      **error);

/* Async calls */
typedef void (*DapiGnomeLocalFileCallback)    (const gchar *filename,
					       GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeUploadFileCallback)   (GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeListCallback)         (GList       *ids,
					       GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeNameCallback)         (const gchar *id,
					       const gchar *given_name,
					       const gchar *family_name,
					       const gchar *full_name,
					       GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeEmailsCallback)       (const gchar *id,
					       GList       *addresses,
					       GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeFindCallback)         (const gchar *name,
					       GList       *ids,
					       GError      *error,
					       gpointer     user_data);
typedef void (*DapiGnomeVCardCallback)        (const gchar *id,
					       const gchar *vcard,
					       GError      *error,
					       gpointer     user_data);

void     dapi_gnome_local_file                (const gchar                  *url,
					       const gchar                  *local,
					       gboolean                      allow_download,
					       DapiGnomeLocalFileCallback    callback,
					       gpointer                      user_data);
void     dapi_gnome_upload_file               (const gchar                  *local,
					       const gchar                  *url,
					       gboolean                      remove_local,
					       DapiGnomeUploadFileCallback   callback,
					       gpointer                      user_data);
gboolean dapi_gnome_remove_temp_local_file    (const gchar                  *filename);
void     dapi_gnome_address_book_get_list     (DapiGnomeListCallback         callback,
					       gpointer                      user_data);
void     dapi_gnome_address_book_get_name     (const gchar                  *id,
					       DapiGnomeNameCallback         callback,
					       gpointer                      user_data);
void     dapi_gnome_address_book_get_emails   (const gchar                  *id,
					       DapiGnomeEmailsCallback       callback,
					       gpointer                      user_data);
void     dapi_gnome_address_book_find_by_name (const gchar                  *name,
					       DapiGnomeFindCallback         callback,
					       gpointer                      user_data);
gboolean dapi_gnome_address_book_get_owner    (gchar                       **id,
					       GError                      **error);
void     dapi_gnome_address_book_get_vcard    (const gchar                  *id,
					       DapiGnomeVCardCallback        callback,
					       gpointer                      user_data);

#endif /* __DAPI_GNOME_H__ */

